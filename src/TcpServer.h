#ifndef HTTP_TCP_SERVER_H
#define HTTP_TCP_SERVER_H

#include <dlfcn.h>
#include <spdlog/common.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <array>
#include <csignal>
#include <filesystem>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "HtmlBuilder.h"
#include "HttpRequestHeader.h"
#include "Middleware.h"
#include "Request.h"
#include "Response.h"
#include "TcpConnection.h"
#include "Threadpool.h"

class TcpServer {
   public:
    static TcpServer &getInstance();

    TcpServer(const TcpServer &) = delete;  // Copy constructor
    TcpServer(TcpServer &&) = delete;       // Move constructor
    TcpServer &operator=(const TcpServer &) =
        delete;                                   // Copy assignment operator
    TcpServer &operator=(TcpServer &&) = delete;  // Move assignment operator

    ~TcpServer();

    void run();
    std::string getIp();

   private:
    typedef std::string Endpoint;
    typedef std::string Method;
    typedef void (*endpoint_t)(Request &, Response &);
    typedef std::unordered_map<Method, endpoint_t> EndpointMethods;

    TcpServer();
    void accept();
    // HttpRequestHeader &completeRequest(HttpRequestHeader &requesst);
    std::optional<HttpRequestHeader> read(TcpConnection &connection);
    Response handleRequest(HttpRequestHeader &request);
    std::optional<std::string> getCorrectPath(
        const std::filesystem::path &path);
    Response handleEndpoint(HttpRequestHeader &request);
    Response handleFile(HttpRequestHeader &request);
    void write(TcpConnection &connection, Response &response);

    void registerSignals(std::vector<int> signals);
    void handleSignal(int signum, siginfo_t *info, void *contex);
    void handleCommands();
    void registerMiddleware(const fs::path &path);
    void registerMiddlewares();
    void runMiddleware(Request &request, Response &response,
                       std::stack<Middleware_t> &middlewareStack);
    void callMiddleware(Request &request, Response &response,
                        endpoint_t endpoint);
    void reloadMiddleware(const std::string &path);

    static std::unique_ptr<TcpServer> m_instance;

    int m_socket;
    bool m_running = true;
    std::mutex m_connectionMutex;
    uint64_t m_numberOfConnection = 0;
    std::mutex m_requestMutex;
    uint64_t m_numberOfRequestHandled = 0;
    Threadpool m_threadpool;

    // Watcher
    volatile std::sig_atomic_t m_signal;
    const std::array<int, 2> m_defaultSignals = {SIGINT, SIGTERM};
    int m_pipeWatchFD = -1;

    // Commands
    std::unordered_map<std::string, void (TcpServer::*)()> m_commands = {
        {"stop", &TcpServer::stop},
        {"kill", &TcpServer::stop},
        {"status", &TcpServer::status},
        {"help", &TcpServer::help}};

    void stop();
    void status();
    void help();

    // Middlewares
    typedef struct MiddlewareInfo_s {
       private:
        void *m_library = nullptr;
        fs::path m_path;

       public:
        std::string name;
        Middleware_t func = nullptr;
        MiddlewareUseFunc_t useFunc = nullptr;
        MiddlewareNameFunc_t nameFunc = nullptr;

        MiddlewareInfo_s() = default;

        MiddlewareInfo_s(const fs::path &path) { open(path); }

        bool open(const fs::path &path) {
            m_path = path;
            m_library = dlopen(m_path.string().c_str(), RTLD_NOW | RTLD_LOCAL);
            if (!m_library) {
                spdlog::error("Failed to load middleware {}: {}",
                              m_path.filename().string(), dlerror());
                return false;
            }
            useFunc = (MiddlewareUseFunc_t)(dlsym(m_library, "use"));
            nameFunc = (MiddlewareNameFunc_t)(dlsym(m_library, "name"));
            if (!useFunc || !nameFunc) {
                spdlog::error("Failed to load middleware {}: {}",
                              m_path.filename().string(), dlerror());
                return false;
            }
            auto [routes, middlewareNames] = useFunc();
            name = nameFunc();
            func = (Middleware_t)(dlsym(m_library, nameFunc().c_str()));
            if (!func) {
                spdlog::error("Failed to load middleware {}: {}",
                              m_path.filename().string(), dlerror());
                return false;
            }
            SPDLOG_INFO("Registering middleware {} for routes {} at {}",
                        nameFunc(), utils::join(routes, ", "), m_library);
            return true;
        }

        bool open() { return open(m_path); }

        bool close() {
            if (!m_library) return false;
            SPDLOG_INFO("Closing middleware {}, {}", name, m_library);
            if (dlclose(m_library)) {
                spdlog::error("Failed to close middleware {}: {}", name,
                              dlerror());
                return false;
            }
            func = nullptr;
            useFunc = nullptr;
            nameFunc = nullptr;
            m_library = nullptr;
            return true;
        }

        bool reload() {
            SPDLOG_INFO("Reloading middleware {}", name);
            close();
            return open();
        }
    } MiddlewareInfo;
    typedef std::string Path;
    std::unordered_map<Path, MiddlewareInfo> m_middlewares;
    typedef std::string RouteRegex;
    typedef std::vector<std::string> Paths;
    std::vector<std::pair<RouteRegex, Paths>> m_middlewareMatcher;

    std::unordered_map<Endpoint, std::pair<void *, EndpointMethods>>
        m_endpoints;

    // Config
    std::string m_host = "localhost";
    std::string m_port = "8081";
    fs::path m_apiFolder = fs::weakly_canonical("./api");
    fs::path m_publicFolder = fs::weakly_canonical("./public");
    fs::path m_middlewareFolder = fs::weakly_canonical("./api/__middleware__");
    uint32_t m_threadCount = 10;
    bool m_watch = false;

    typedef std::string ConfigKey;
    std::unordered_map<ConfigKey, void (TcpServer::*)(nlohmann::json &)>
        m_configHandlers = {
            {"host", &TcpServer::handleHostConfig},
            {"port", &TcpServer::handlePortConfig},
            {"api", &TcpServer::handleApiConfig},
            {"public", &TcpServer::handlePublicConfig},
            {"middleware", &TcpServer::handleMiddlewareConfig},
            {"numberOfThread", &TcpServer::handleNumberOfThread},
            {"watch", &TcpServer::handleWatchConfig},
            {"log", &TcpServer::handleLogConfig}};

    void handleHostConfig(nlohmann::json &host);
    void handlePortConfig(nlohmann::json &port);
    void handleApiConfig(nlohmann::json &api);
    void handlePublicConfig(nlohmann::json &p);
    void handleMiddlewareConfig(nlohmann::json &p);
    void handleNumberOfThread(nlohmann::json &numberOfThread);
    void handleWatchConfig(nlohmann::json &watch);
    void handleLogConfig(nlohmann::json &log);
};

#endif
