#include "../TcpServer.h"
#include "../HttpRequestHeader.h"
#include "../HttpResponseHeader.h"
#include "../utils.h"
#include "../Logger.h"

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

TcpServer& TcpServer::getInstance() {
    if (!m_instance)
        m_instance = std::unique_ptr<TcpServer>(new TcpServer());
    return *m_instance;
}

TcpServer::TcpServer() {
    std::ifstream config("./config.json");
    if (config.is_open())
        config >> m_config;
    for (auto& [key, value] : m_config.items()) {
        if (m_configHandlers.find(key) != m_configHandlers.end())
            (this->*m_configHandlers[key])(value);
    }
    registerSignals(m_defaultSignals);

    addrinfo hints;
    addrinfo *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int status = getaddrinfo(m_host.c_str(), m_port.c_str(), &hints, &result);
    if (status != 0)
        throw std::runtime_error("getaddrinfo() failed: " + std::string(strerror(errno)));
    m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (m_socket == -1) {
        freeaddrinfo(result);
        throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));
    }
    int optval = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    status = bind(m_socket, result->ai_addr, result->ai_addrlen);
    if (status == -1) {
        freeaddrinfo(result);
        close(m_socket);
        perror("bind() failed");
        throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));
    }
    freeaddrinfo(result);
    status = listen(m_socket, 10);
    if (status == -1) {
        close(m_socket);
        throw std::runtime_error("listen() failed: " + std::string(strerror(errno)));
    }
}

TcpServer::~TcpServer() {
    for (const auto& [_path, lib] : m_endpoints)
        dlclose(lib.first);
    close(m_socket);
    close(m_pipeFD);
}

void TcpServer::run() {
    if (spdlog::get_level() == spdlog::level::debug) SPDLOG_WARN("Server is running in debug mode");
    SPDLOG_INFO("Server started");
    utils::initializeMimeMap();
    while (m_running)
        accept();
    SPDLOG_INFO("Server stopped");
}

void TcpServer::accept() {
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    int clientSocket = ::accept(m_socket, (sockaddr*)&client, &clientSize);
    if (clientSocket == -1 && errno != EINTR)
        throw std::runtime_error("accept() failed: " + std::string(strerror(errno)));

    std::thread([clientSocket, this] () {
        SPDLOG_WARN("New connection created");
        TcpConnection connection(clientSocket);
        while (connection.isOpen()) {
            auto requestHeader = read(connection);
            auto start = std::chrono::high_resolution_clock::now();
            if (!requestHeader)
                continue;
            HttpResponseHeader responseHeader = handleRequest(requestHeader.value());
            write(connection, responseHeader);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            LoggedInfo info(requestHeader.value(), responseHeader, duration.count());
            std::vector<std::string> toPrint;
            for (const auto &format : LOGGER_FORMAT)
                toPrint.push_back(fmt::format(format, fmt::arg(format.c_str(), info)));
            spdlog::get("RouteLogger")->info(utils::join(toPrint, " "));
            if (requestHeader.value().getHeader("Connection").has_value() && requestHeader.value().getHeader("Connection").value() != "keep-alive")
                break;
            SPDLOG_INFO("Connection keep-alive");
        }
        SPDLOG_WARN("Connection closed");
        close(clientSocket);
    }).detach();
}