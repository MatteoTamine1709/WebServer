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
    TcpServer();
    void accept();
    // HttpRequestHeader &completeRequest(HttpRequestHeader &requesst);
    std::optional<HttpRequestHeader> read(TcpConnection& connection);
    // HttpResponseHeader &completeResponse(HttpResponseHeader &response, const HttpRequestHeader &request);
    Response handleRequest(HttpRequestHeader& request);
    std::optional<std::string> getCorrectPath(const std::filesystem::path &path);
    Response handleEndpoint(HttpRequestHeader& request);
    Response handleFile(HttpRequestHeader& request);
    void write(TcpConnection& connection, const Response& response);

    void registerSignals(std::vector<int> signals);
    void handleSignal(int signum, siginfo_t *info, void *contex);

    static std::unique_ptr<TcpServer> m_instance;

    int m_socket;

    volatile std::sig_atomic_t m_signal;
    const std::array<int, 2> m_defaultSignals = {SIGINT, SIGTERM};

    int m_pipeFD = -1;
    bool m_running = true;

    typedef std::string Endpoint;
    typedef std::string Method;
    typedef void (*endpoint_t)(const Request &, Response &response);
    typedef std::unordered_map<Method, endpoint_t> EndpointMethods;
    std::unordered_map<Endpoint, std::pair<void *, EndpointMethods>> m_endpoints;

    std::string m_host = "localhost";
    std::string m_port = "8081";
    fs::path m_apiFolder = "./api";
    fs::path m_publicFolder = "./public";
    bool m_watch = false;

    uint64_t m_numberThreads = 0;
    std::mutex m_mutex;

    typedef std::string ConfigKey;
    std::unordered_map<ConfigKey, void (TcpServer::*)(nlohmann::json &)> m_configHandlers = {
            {"host", &TcpServer::handleHostConfig},
            {"port", &TcpServer::handlePortConfig},
            {"api", &TcpServer::handleApiConfig},
            {"public", &TcpServer::handlePublicConfig},
            {"watch", &TcpServer::handleWatchConfig},
            {"log", &TcpServer::handleLogConfig}
    };

    void handleHostConfig(nlohmann::json &host);
    void handlePortConfig(nlohmann::json &port);
    void handleApiConfig(nlohmann::json &api);
    void handlePublicConfig(nlohmann::json &p);
    void handleWatchConfig(nlohmann::json &watch);
    void handleLogConfig(nlohmann::json &log);
};

#endif
