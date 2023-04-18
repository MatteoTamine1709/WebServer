#include "../TcpServer.h"
#include "../HttpRequestHeader.h"
#include "../HttpResponseHeader.h"
#include "../utils.h"

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
#include <filesystem>

#include <dlfcn.h>
#include <fcntl.h>

std::unique_ptr<TcpServer> TcpServer::m_instance = nullptr;
volatile std::sig_atomic_t TcpServer::m_signal = -1;

TcpServer& TcpServer::getInstance() {
    if (!m_instance)
        m_instance = std::unique_ptr<TcpServer>(new TcpServer());
    return *m_instance;
}

TcpServer::TcpServer() {
    std::ifstream config("./config.json");
    if (config.is_open())
        config >> m_config;
    std::cout << m_config.dump(2) << std::endl;
    for (auto& [key, value] : m_config.items()) {
        if (m_configHandlers.find(key) != m_configHandlers.end())
            (this->*m_configHandlers[key])(value);
    }
    registerSignals(m_defaultSignalHandlers);

    addrinfo hints;
    addrinfo* result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int status = getaddrinfo(m_host.c_str(), m_port.c_str(), &hints, &result);
    if (status != 0)
        throw std::runtime_error("getaddrinfo() failed");
    m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (m_socket == -1) {
        freeaddrinfo(result);
        throw std::runtime_error("socket() failed");
    }
    int optval = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
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
    spdlog::info("Server started");
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

    std::thread([clientSocket, this](){
        TcpConnection connection(clientSocket);
        while (connection.isOpen()) {
            auto requestHeader = read(connection);
            if (!requestHeader)
                continue;
            HttpResponseHeader responseHeader = handleRequest(requestHeader.value());
            write(connection, responseHeader);
            if (requestHeader.value().getHeader("Connection").has_value() && requestHeader.value().getHeader("Connection").value() != "keep-alive")
                break;
        }
        close(clientSocket);
    }).detach();
}