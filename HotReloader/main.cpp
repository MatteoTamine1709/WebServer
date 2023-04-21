#include "src/File.h"
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <thread>

void loadFiles(std::string path);
bool running = true;
int signalFD;
std::string path;
bool connected = false;
pid_t targetPID = -1;

int openPipe(const std::string &path) {
    if (mkfifo(path.c_str(), 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        std::cerr << "Failed to create named pipe." << std::endl;
        return 1;
    }
    int fd = open(path.c_str(), O_RDWR);
    if (fd == -1) {
        std::cerr << "Failed to open named pipe." << std::endl;
        return 1;
    }
    return fd;
}

void sendMyPID() {
    pid_t pid = getpid();
    write(signalFD, &pid, sizeof(pid_t));
    kill(targetPID, SIGUSR1);
}

void signalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "Stopping" << std::endl; 
        close(signalFD);
        kill(targetPID, SIGUSR1);
        running = false;
        exit(0);
    }
    if (signal == SIGUSR1) {
        char buffer[1024];
        size_t n_read = read(signalFD, buffer, 1024);
        if (n_read == -1) {
            std::cerr << "Failed to read from pipe." << std::endl;
            return;
        }
        buffer[n_read] = '\0';
        path = buffer;
        path += "/";
        std::cout << "Open " << path << std::endl;
        connected = true;
    }

}

int connection() {
    FILE *command = popen("pidof WebServer","r");
    char line[1024];
    pid_t previousPID = targetPID;
    fgets(line, 1024, command);
    targetPID = strtoul(line, NULL, 10);
    pclose(command);
    if (targetPID == previousPID) {
        sleep(5);
        return 0;
    }
    connected = false;
    sendMyPID();
    while (!connected) {
        std::cout << "Waiting for connection..." << std::endl;
        sleep(1);
    }
    std::cout << "Connected to " << targetPID << std::endl;
    kill(targetPID, SIGUSR1);
    return 1;
}

int main(int argc, char **argv) {
    struct sigaction sa = {};
    sa.sa_handler = signalHandler;
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGUSR1, &sa, nullptr);
    sigaction(SIGUSR2, &sa, nullptr);

    const std::string fifoPipePath = "/tmp/fifo";
    signalFD = openPipe(fifoPipePath);

    connection();
    std::thread([&]() { 
        loadFiles(path);
        for (auto &[path, file] : files)
            file.compile();
        while (running) {
            if (connection()) {
                loadFiles(path);
                for (auto &[path, file] : files)
                    file.compile();
            }

        }
    }).detach();
    while (running) {
        if (!connected) {
            sleep(1);
            continue;
        }
        loadFiles(path);
        for (auto &[path, file] : files) {
            if (file.isUpdated()) {
                file.compile();
                // Signal to the project that the file has been updated
                write(signalFD, file.toAbsolutePath().c_str(), file.toAbsolutePath().length());
                if (kill(targetPID, SIGUSR2) == -1) {
                    std::cerr << "Failed to send signal to target process." << std::endl;
                    return 1;
                }
            }
        }    
    }
    return 0;
}
