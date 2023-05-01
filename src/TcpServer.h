#ifndef HTTP_TCP_SERVER_H
#define HTTP_TCP_SERVER_H

#include <memory>
#include <string>
#include <vector>
#include <array>
#include <csignal>
#include <filesystem>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/common.h>

#include "TcpConnection.h"
#include "HttpRequestHeader.h"
#include "Request.h"
#include "Response.h"
#include "Middleware.h"

class TcpServer {
public:
    static TcpServer& getInstance();

    TcpServer(const TcpServer&) = delete; // Copy constructor
    TcpServer(TcpServer&&) = delete; // Move constructor
    TcpServer& operator=(const TcpServer&) = delete; // Copy assignment operator
    TcpServer& operator=(TcpServer&&) = delete; // Move assignment operator

    ~TcpServer();

    void run();
    std::string getIp();

private:

    typedef std::string Endpoint;
    typedef std::string Method;
    typedef void (*endpoint_t)(const Request &, Response &);
    typedef std::unordered_map<Method, endpoint_t> EndpointMethods;

    TcpServer();
    void accept();
    // HttpRequestHeader &completeRequest(HttpRequestHeader &requesst);
    std::optional<HttpRequestHeader> read(TcpConnection& connection);
    Response handleRequest(HttpRequestHeader& request);
    std::optional<std::string> getCorrectPath(const std::filesystem::path &path);
    Response handleEndpoint(HttpRequestHeader& request);
    Response handleFile(HttpRequestHeader& request);
    void write(TcpConnection& connection, Response& response);

    void registerSignals(std::vector<int> signals);
    void handleSignal(int signum, siginfo_t *info, void *contex);
    void handleCommands();
    void registerMiddlewares();
    void runMiddleware(const Request &request, Response &response, std::stack<Middleware_t> &middlewareStack);
    void callMiddleware(const Request &request, Response &response, endpoint_t endpoint);


    static std::unique_ptr<TcpServer> m_instance;

    int m_socket;

    volatile std::sig_atomic_t m_signal;
    const std::array<int, 2> m_defaultSignals = {SIGINT, SIGTERM};

    std::unordered_map<std::string, void (TcpServer::*)()> m_commands = {
        {"stop", &TcpServer::stop},
        {"kill", &TcpServer::stop},
        {"status", &TcpServer::status},
        {"help", &TcpServer::help}
    };

    void stop();
    void status();
    void help();

    std::unordered_map<std::string, Middleware_t> m_middlewares;
    std::unordered_map<std::string, void *> m_middlewareLibraries;
    std::vector<std::pair<std::string, std::vector<std::string>>> m_middlewareMatcher;

    int m_pipeFD = -1;
    bool m_running = true;

    std::unordered_map<Endpoint, std::pair<void *, EndpointMethods>> m_endpoints;

    std::string m_host = "localhost";
    std::string m_port = "8081";
    fs::path m_apiFolder = "./api";
    fs::path m_publicFolder = "./public";
    fs::path m_middlewareFolder = "./__middleware__";
    bool m_watch = false;

    uint64_t m_numberConnections = 0;
    uint64_t m_numberRequests = 0;
    std::mutex m_mutex;

    typedef std::string ConfigKey;
    std::unordered_map<ConfigKey, void (TcpServer::*)(nlohmann::json &)> m_configHandlers = {
            {"host", &TcpServer::handleHostConfig},
            {"port", &TcpServer::handlePortConfig},
            {"api", &TcpServer::handleApiConfig},
            {"public", &TcpServer::handlePublicConfig},
            {"middleware", &TcpServer::handleMiddlewareConfig},
            {"watch", &TcpServer::handleWatchConfig},
            {"log", &TcpServer::handleLogConfig}
    };

    void handleHostConfig(nlohmann::json &host);
    void handlePortConfig(nlohmann::json &port);
    void handleApiConfig(nlohmann::json &api);
    void handlePublicConfig(nlohmann::json &p);
    void handleMiddlewareConfig(nlohmann::json &p);
    void handleWatchConfig(nlohmann::json &watch);
    void handleLogConfig(nlohmann::json &log);
};

#endif
