#include "src/File.h"
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

void loadFiles(std::string path);

int openPipe(const std::string &path) {
    if (mkfifo(path.c_str(), 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        std::cerr << "Failed to create named pipe." << std::endl;
        return 1;
    }
    int fd = open(path.c_str(), O_WRONLY | O_NONBLOCK);
    if (fd == -1) {
        std::cerr << "Failed to open named pipe." << std::endl;
        return 1;
    }
    return fd;
}

bool running = true;

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <path> <targetPID>" << std::endl;
        return 1;
    }
    loadFiles(argv[1]);
    pid_t targetPID = std::stoi(argv[2]);

    const std::string fifoPipePath = "/tmp/fifo";
    int signalFD = openPipe(fifoPipePath);

    struct sigaction sa = {};
    sa.sa_handler = [](int) { running = false; };
    sigaction(SIGINT, &sa, nullptr);

    while (running) {
        for (auto &[path, file] : files) {
            if (file.isUpdated()) {
                file.compile();
                // Signal to the project that the file has been updated
                write(signalFD, file.toAbsolutePath().c_str(), file.toAbsolutePath().length());
                if (kill(targetPID, SIGUSR1) == -1) {
                    std::cerr << "Failed to send signal to target process." << std::endl;
                    return 1;
                }
            }
        }
    }
    std::cout << "Stopping" << std::endl; 
    close(signalFD);
    return 0;
}
