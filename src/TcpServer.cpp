#include "TcpServer.h"
#include "HttpRequestHeader.h"
#include "HttpResponseHeader.h"
#include "utils.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <thread>
#include <netdb.h>
#include <cstring>
#include <signal.h>

#include <fstream>
#include <sstream>

#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <filesystem>

std::unique_ptr<TcpServer> TcpServer::m_instance = nullptr;
volatile std::sig_atomic_t TcpServer::m_signal = -1;

TcpServer& TcpServer::getInstance(const std::string& address, const std::string& port) {
    if (!m_instance)
        m_instance = std::unique_ptr<TcpServer>(new TcpServer(address, port));
    return *m_instance;
}

TcpServer::TcpServer(const std::string& address, const std::string& port) {
    std::thread(&TcpServer::hotReload, this).detach();
    std::thread(&TcpServer::stopServer, this).detach();
    struct sigaction sa;
    sa.sa_sigaction = &TcpServer::handleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    const char *fifo_path = "/tmp/fifo";

    if (mkfifo(fifo_path, 0666) == -1 && errno != EEXIST) {
        std::cerr << "Failed to create named pipe." << std::endl;
        return;
    }

    m_pipeFD = open(fifo_path, O_RDONLY); // Open in non-blocking mode

    addrinfo hints;
    addrinfo* result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int status = getaddrinfo(address.c_str(), port.c_str(), &hints, &result);
    if (status != 0) {
        throw std::runtime_error("getaddrinfo() failed");
    }
    m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (m_socket == -1) {
        freeaddrinfo(result);
        throw std::runtime_error("socket() failed");
    }
    int a = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(int));
    status = bind(m_socket, result->ai_addr, result->ai_addrlen);
    if (status == -1) {
        freeaddrinfo(result);
        close(m_socket);
        throw std::runtime_error("bind() failed");
    }
    freeaddrinfo(result);
    status = listen(m_socket, 10);
    if (status == -1) {
        close(m_socket);
        throw std::runtime_error("listen() failed");
    }
}

TcpServer::~TcpServer() {
    std::cout << "Closing server" << std::endl;
    for (auto& lib : m_endpointLibs)
        dlclose(lib.second);
    close(m_socket);
    close(m_pipeFD);
}

void TcpServer::run() {
    utils::initializeMimeMap();
    while (m_running)
        accept();
}

void TcpServer::accept() {
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    int clientSocket = ::accept(m_socket, (sockaddr*)&client, &clientSize);
    if (clientSocket == -1 && errno != EINTR)
        throw std::runtime_error("accept() failed");
    std::cout << "New connection" << std::endl;

    std::thread t([clientSocket, this](){
        TcpConnection connection(clientSocket);
        while (connection.isOpen()) {
            auto requestHeader = read(connection);
            if (!requestHeader)
                continue;
            HttpResponseHeader responseHeader = handleRequest(requestHeader.value());
            write(connection, responseHeader);
            // Unsure if this is the best way to handle keep-alive connections
            if (requestHeader.value().getHeader("Connection").has_value() && requestHeader.value().getHeader("Connection").value() != "keep-alive")
                break;
        }
        close(clientSocket);
    });
    t.detach();
}

std::optional<HttpRequestHeader> TcpServer::read(TcpConnection& connection) {
    const std::string &data = connection.read();
    if (data.empty())
        return std::nullopt;
    
    HttpRequestHeader header(data);

    return header;
}

HttpResponseHeader &TcpServer::completeResponse(HttpResponseHeader &response, const HttpRequestHeader &request) {
    const std::vector<std::string> &allowedMethods = {"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "CONNECT", "TRACE"};
    response.setHeader("Server", "MyCustomServer/1.0");
    response.setHeader("Date", utils::getDateTime());
    if (request.getMethod() == "options" && !response.getHeader("Allow").has_value())
        response.setHeader("Allow", utils::join(allowedMethods, ", "));
    if (request.getHeader("Connection").has_value() && !response.getHeader("Connection").has_value())
        response.setHeader("Connection", request.getHeader("Connection").value());
    if (request.getHeader("Cache-Control").has_value() && !response.getHeader("Cache-Control").has_value())
        response.setHeader("Cache-Control", request.getHeader("Cache-Control").value());
    return response;
}

HttpResponseHeader TcpServer::handleRequest(HttpRequestHeader &request) {
    // TODO: Handle parematerized paths
    // TODO: Handle request parameters
    if (request.getMethod() == "get" && !utils::getExtension(request.getPath()).empty()) {
        request.setPath("/public" + request.getPath());
        if (!request.isPathValid())
            return HttpResponseHeader("HTTP/1.1", "404", "Not Found", "Not Found");
        std::cout << "Canonical path: " << request.getCanonicalPath() << std::endl;
        HttpResponseHeader response = handleFile(request);
        completeResponse(response, request);
        return response;
    }
    request.setPath("/endpoints" + request.getPath() + "/index.so");
    if (!request.isPathValid())
        return HttpResponseHeader("HTTP/1.1", "404", "Not Found", "Not Found");
    std::cout << "Canonical path: " << request.getCanonicalPath() << std::endl;
    HttpResponseHeader response = handleEndpoint(request);
    completeResponse(response, request);
    return response;
}

HttpResponseHeader TcpServer::handleEndpoint(HttpRequestHeader &request) {
    if (m_endpointHandlers.find(request.getCanonicalPath()) != m_endpointHandlers.end() &&
        m_endpointHandlers[request.getCanonicalPath()].find(request.getMethod()) != m_endpointHandlers[request.getCanonicalPath()].end()) {
        endpoint_t endpoint = m_endpointHandlers[request.getCanonicalPath()][request.getMethod()];
        return endpoint(request);
    }
    void *lib = nullptr;
    if (m_endpointLibs.find(request.getCanonicalPath()) != m_endpointLibs.end()) {
        lib = m_endpointLibs[request.getCanonicalPath()];
    } else {
        lib = dlopen(request.getCanonicalPath().c_str(), RTLD_LAZY);
        if (!lib) {
            std::cerr << "Cannot open library: " << dlerror() << '\n';
            HttpResponseHeader response("HTTP/1.1", "404", "Not Found", "Not Found");
            response.setHeader("Content-Type", "text/html");
            return response;
        }
        m_endpointLibs[request.getCanonicalPath()] = lib;
    }
    // Load the symbols
    endpoint_t endpoint = (endpoint_t) dlsym(lib, request.getMethod().c_str());
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol '" << request.getMethod() << "': " << dlsym_error << '\n';
        dlclose(lib);
        HttpResponseHeader response("HTTP/1.1", "404", "Not Found", "Not Found");
        response.setHeader("Content-Type", "text/html");
        return response;
    }

    HttpResponseHeader response;
    try {
        response = endpoint(request);
        // Cache the endpoint
        m_endpointHandlers[request.getCanonicalPath()][request.getMethod()] = endpoint;
        return response;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        dlclose(lib);
        HttpResponseHeader response("HTTP/1.1", "500", "Internal Server Error", "Internal Server Error");
        response.setHeader("Content-Type", "text/html");
        return response;
    }
}

HttpResponseHeader TcpServer::handleFile(HttpRequestHeader &request) {
    std::ifstream file(request.getCanonicalPath().c_str(), std::ios::binary);
    if (!file.is_open()) {
        HttpResponseHeader response("HTTP/1.1", "404", "Not Found", "Not Found");
        response.setHeader("Content-Type", "text/html");
        return response;
    }

    file.seekg(0, std::ios::end);
    const std::streamsize filesize = file.tellg();
    file.seekg(0, std::ios::beg);
    HttpResponseHeader response("HTTP/1.1", "200", "OK", "OK");
    response.setHeader("Content-Type", utils::getContentType(request.getCanonicalPath()));
    std::stringstream ss;
    ss << file.rdbuf();
    response.setBody(ss.str());
    return response;
}

void TcpServer::write(TcpConnection& connection, const HttpResponseHeader& response) {
    // std::cout << response.buildReadableResponse() << std::endl;
    connection.write(response.buildResponse());
    // std::ifstream file("assets/videos/Studio_Project.mp4", std::ios::binary);
    // file.seekg(0, std::ios::end);
    // const std::streamsize filesize = file.tellg();
    // file.seekg(0, std::ios::beg);
    // std::ostringstream header;
    // header << "HTTP/1.1 200 OK\r\n"
    //         << "Content-Type: video/mp4\r\n"
    //         << "Content-Length: " << 4 * 1024 * 1024 << "\r\n"
    //         << "Connection: close\r\n"
    //         << "\r\n";
    // // std::cout << content << std::endl;

    // const std::string header_str = header.str();
    // connection.write(header_str);
    // std::vector<char> buffer(1 * 1024 * 1024); // 1MB
    // // while (file.good()) {
    //     file.read(buffer.data(), buffer.size());
    //     const std::streamsize count = file.gcount();
    //     // if (count == 0)
    //     //     break;
    //     connection.write(std::string(buffer.data(), count));
    // }
}

void TcpServer::handleSignal(int signum, siginfo_t *info, void *context) {
    m_signal = signum;
}

void TcpServer::hotReload() {
    while (true) {
        if (m_signal == SIGUSR1) {
            char hotReloaded_path[1024];
            ssize_t n_read = ::read(m_pipeFD, hotReloaded_path, sizeof(hotReloaded_path));
            if (n_read == -1) {
                perror("read");
                std::cerr << "Failed to read data from named pipe." << std::endl;
                continue;
            }
            hotReloaded_path[n_read] = '\0';

            std::filesystem::path path(hotReloaded_path);
            path.replace_extension(".so");
            std::string canonical = std::filesystem::canonical(path);
            if (m_endpointLibs.find(canonical) != m_endpointLibs.end())
                dlclose(m_endpointLibs[canonical]);
            void *lib = dlopen(canonical.c_str(), RTLD_LAZY);
            if (!lib) {
                std::cerr << "Cannot open library: " << dlerror() << '\n';
                continue;
            }
            m_endpointLibs[canonical] = lib;
            std::cout << "Hot reloaded: " << canonical << std::endl;
        }
    }
}

void TcpServer::stopServer() {
    while (true) {
        if (m_signal == SIGINT)
            m_running = false;
    }
}