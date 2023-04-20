#ifndef HTTP_TCP_SERVER_H
#define HTTP_TCP_SERVER_H

#include <memory>
#include <string>
#include <vector>
#include <csignal>
#include <filesystem>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/common.h>

#include "TcpConnection.h"
#include "HttpRequestHeader.h"
#include "HttpResponseHeader.h"

class TcpServer {
public:
    static TcpServer& getInstance();

    TcpServer(const TcpServer&) = delete; // Copy constructor
    TcpServer(TcpServer&&) = delete; // Move constructor
    TcpServer& operator=(const TcpServer&) = delete; // Copy assignment operator
    TcpServer& operator=(TcpServer&&) = delete; // Move assignment operator

    ~TcpServer();

    void run();

private:
    TcpServer();
    void accept();
    HttpRequestHeader &completeRequest(HttpRequestHeader &requesst);
    std::optional<HttpRequestHeader> read(TcpConnection& connection);
    HttpResponseHeader &completeResponse(HttpResponseHeader &response, const HttpRequestHeader &request);
    HttpResponseHeader handleRequest(HttpRequestHeader& request);
    std::optional<std::string> getCorrectPathOrEmpty(const std::filesystem::path &path);
    HttpResponseHeader handleEndpoint(HttpRequestHeader& request);
    HttpResponseHeader handleFile(HttpRequestHeader& request);
    void write(TcpConnection& connection, const HttpResponseHeader& response);

    void registerSignals(std::vector<int> signals);
    void handleSignal(int signum, siginfo_t *info, void *contex);

    static std::unique_ptr<TcpServer> m_instance;

    int m_socket;

    volatile std::sig_atomic_t m_signal;
    std::vector<int> m_defaultSignals = {SIGINT, SIGTERM};

    int m_pipeFD = -1;
    bool m_watch = false;
    bool m_running = true;

    typedef std::string Endpoint;
    typedef std::string Method;
    typedef std::string LibPath;
    typedef HttpResponseHeader (*endpoint_t)(HttpRequestHeader);
    typedef std::unordered_map<Method, endpoint_t> EndpointMethods;
    std::unordered_map<Endpoint, std::pair<void *, EndpointMethods>> m_endpoints;

    std::string m_host = "localhost";
    std::string m_port = "8081";
    std::string m_apiFolder = "/api";
    std::string m_publicFolder = "/public";
    nlohmann::json m_config = {};
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
