#include "../TcpServer.h"

#include <arpa/inet.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include "../Logger.h"
#include "../Response.h"
#include "../utils.h"

std::unique_ptr<TcpServer> TcpServer::m_instance = nullptr;

TcpServer& TcpServer::getInstance() {
    if (!m_instance) m_instance = std::unique_ptr<TcpServer>(new TcpServer());
    return *m_instance;
}

TcpServer::TcpServer() {
    std::ifstream configFile("./config.json");
    if (configFile.is_open()) {
        nlohmann::json config;
        configFile >> config;
        for (auto& [key, value] : config.items()) {
            if (m_configHandlers.find(key) != m_configHandlers.end())
                (this->*m_configHandlers[key])(value);
        }
        configFile.close();
    }
    m_threadpool.setThreadCount(m_threadCount);
    registerSignals(
        std::vector(std::begin(m_defaultSignals), std::end(m_defaultSignals)));
    std::thread([this]() { handleCommands(); }).detach();
    registerMiddlewares();

    addrinfo hints;
    addrinfo* result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int status = getaddrinfo(m_host.c_str(), m_port.c_str(), &hints, &result);
    if (status != 0)
        throw std::runtime_error("getaddrinfo() failed: " +
                                 std::string(strerror(errno)));
    m_socket =
        socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (m_socket == -1) {
        freeaddrinfo(result);
        throw std::runtime_error("socket() failed: " +
                                 std::string(strerror(errno)));
    }
    int optval = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    status = bind(m_socket, result->ai_addr, result->ai_addrlen);
    if (status == -1) {
        freeaddrinfo(result);
        close(m_socket);
        perror("bind() failed");
        throw std::runtime_error("bind() failed: " +
                                 std::string(strerror(errno)));
    }
    freeaddrinfo(result);
    status = listen(m_socket, 10);
    if (status == -1) {
        close(m_socket);
        throw std::runtime_error("listen() failed: " +
                                 std::string(strerror(errno)));
    }
}

TcpServer::~TcpServer() {
    for (const auto& [_path, lib] : m_endpoints) dlclose(lib.first);
    for (auto& [_path, middlewareInfo] : m_middlewares) middlewareInfo.close();
    close(m_socket);
    close(m_pipeWatchFD);
}

void TcpServer::run() {
    if (spdlog::get_level() == spdlog::level::debug)
        SPDLOG_WARN("Server is running in debug mode");
    SPDLOG_INFO("Server started");
    utils::initializeMimeMap();
    m_threadpool.start();
    while (m_running) accept();
    m_threadpool.stop();
    SPDLOG_INFO("Server stopped");
}

std::string TcpServer::getIp() {
    char myIP[16];
    struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr));
    socklen_t len = sizeof(my_addr);
    getsockname(m_socket, (struct sockaddr*)&my_addr, &len);
    inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
    return std::string(myIP);
}

fs::path TcpServer::getApiPath() const { return m_apiFolder; }

void TcpServer::accept() {
    fd_set readFds;
    FD_ZERO(&readFds);
    FD_SET(m_socket, &readFds);
    timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    int status = select(m_socket + 1, &readFds, NULL, NULL, &timeout);
    if (status == -1 && errno != EINTR) {
        throw std::runtime_error("select() failed: " +
                                 std::string(strerror(errno)));
    }
    if (status == 0 || status == -1) return;
    sockaddr_in client = {0};
    socklen_t clientSize = sizeof(client);
    int clientSocket = ::accept(m_socket, (sockaddr*)&client, &clientSize);
    if (clientSocket == -1 && errno != EINTR)
        throw std::runtime_error("accept() failed: " +
                                 std::string(strerror(errno)));
    SPDLOG_INFO("New connection from {}:{}", inet_ntoa(client.sin_addr),
                ntohs(client.sin_port));
    m_connectionMutex.lock();
    m_numberOfConnection++;
    m_connectionMutex.unlock();
    std::thread([clientSocket, this]() {
        spdlog::debug("New task for client {}", clientSocket);
        TcpConnection connection(clientSocket);
        while (connection.isOpen()) {
            auto requestHeader = this->readHeader(connection);
            if (!requestHeader) continue;
            m_threadpool.addTask(
                [requestHeader = std::move(requestHeader.value()), &connection,
                 clientSocket, this]() mutable {
                    auto start = std::chrono::high_resolution_clock::now();
                    spdlog::debug("New task for client {}", clientSocket);
                    Response responseHeader = handleRequest(requestHeader);
                    write(connection, responseHeader);
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration =
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            end - start);
                    LoggedInfo info(responseHeader, duration.count());
                    std::vector<std::string> toPrint;
                    for (const auto& format : LOGGER_FORMAT)
                        toPrint.push_back(fmt::format(
                            format, fmt::arg(format.c_str(), info)));
                    spdlog::get("RouteLogger")->info(utils::join(toPrint, " "));
                    if (requestHeader["Connection"].has_value() &&
                        requestHeader["Connection"].value() != "keep-alive")
                        close(clientSocket);
                    m_requestMutex.lock();
                    m_numberOfRequestHandled++;
                    m_requestMutex.unlock();
                });
        };
        close(clientSocket);
        m_connectionMutex.lock();
        m_numberOfConnection--;
        m_connectionMutex.unlock();
    }).detach();
}
