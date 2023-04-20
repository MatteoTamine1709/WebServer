#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include "../TcpServer.h"
#include "../HttpRequestHeader.h"
#include "../HttpResponseHeader.h"
#include "../utils.h"

#include <thread>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>

#include <dlfcn.h>
#include <fcntl.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>

#include "../Logger.h"

std::vector<std::string> LOGGER_FORMAT = {};

void TcpServer::handleHostConfig(nlohmann::json &host) {
    if (host.is_string())
        m_host = host;
}

void TcpServer::handlePortConfig(nlohmann::json &port) {
    if (port.is_number())
        m_port = std::to_string(port.get<int>());
    if (port.is_string())
        m_port = port;
}

void TcpServer::handleApiConfig(nlohmann::json &api) {
    if (api.is_string())
        m_api = std::filesystem::canonical(api);
}

void TcpServer::handlePublicConfig(nlohmann::json &p) {
    if (p.is_string())
        m_public = std::filesystem::canonical(p);
}

void TcpServer::handleWatchConfig(nlohmann::json &watch) {
    if (watch.is_boolean() && watch.get<bool>()) {
        const char *fifo_path = "/tmp/fifo";

        if (mkfifo(fifo_path, 0666) == -1 && errno != EEXIST)
            return SPDLOG_ERROR("Failed to create FIFO: {}", strerror(errno));
        spdlog::debug("Created FIFO at {}", fifo_path);
        SPDLOG_WARN("Waiting for hot reloader to connect...");
        m_pipeFD = open(fifo_path, O_RDONLY);
        if (m_pipeFD == -1)
            return SPDLOG_ERROR("Failed to open FIFO: {}", strerror(errno));
        spdlog::debug("Opened FIFO at {}", fifo_path);
        SPDLOG_INFO("Hot reloader connecting, starting the server...");
        m_watch = true;
    }
    registerSignals({SIGUSR1, SIGUSR2});
}

void setFormat(std::string format) {
    std::unordered_map<std::string, std::string> format_map = {
        {"dev", "{:method} {:url} {:status} {:response-time}ms - {:header[Content-Length]}"},
        {"combined", ":remote-addr - :remote-user [:date[clf]] \":method :url HTTP/:http-version\" :status :res[Content-Length] \":referrer\" \":user-agent\""},
        {"common", ":remote-addr - :remote-user [:date[clf]] \":method :url HTTP/:http-version\" :status :res[Content-Length]"},
        {"dev", ":method :url :status :response-time ms - :res[Content-Length]"},
        {"short", ":remote-addr :remote-user :method :url HTTP/:http-version :status :res[Content-Length] - :response-time ms"},
        {"tiny", ":method :url :status :res[Content-Length] - :response-time ms"}
    };

    if (format_map.find(format) != format_map.end()) {
        LOGGER_FORMAT = utils::split(format_map[format], {" "});
    } else {
        LOGGER_FORMAT = utils::split(format, {" "});
    }
}

void TcpServer::handleLogConfig(nlohmann::json &log) {
    
    if (log.is_string() && log == "off") return spdlog::set_level(spdlog::level::off);
    if (log.is_string()) spdlog::set_level(spdlog::level::from_str(log));
    if (log.is_number()) spdlog::set_level(static_cast<spdlog::level::level_enum>(log));

    if (log.is_object()) {
        std::string format = "%:";
        if (log.contains("format") && log["format"].is_string())
            setFormat(log["format"].get<std::string>());
        if (log.contains("timestamp") && log["timestamp"].is_boolean() && log["timestamp"].get<bool>())
            format = "[%Y-%m-%d %H:%M:%S.%e] " + format;

        std::vector<spdlog::sink_ptr> sinks;
        std::vector<spdlog::sink_ptr> routeSinks;
        if (!log.contains("no-stdout") || !log["no-stdout"].is_boolean() || !log["no-stdout"].get<bool>()) {
            auto routeLogger =  std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto routeFormatter = std::make_unique<spdlog::pattern_formatter>();
            routeFormatter->add_flag<devFomat>(':');
            routeFormatter->set_pattern(format);
            routeLogger->set_level(spdlog::level::info);
            routeLogger->set_formatter(std::move(routeFormatter));
            routeSinks.push_back(routeLogger);
            sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
        }
        if (log.contains("file") && log["file"].is_string()) {
            auto fileRouteLogger = std::make_shared<spdlog::sinks::daily_file_sink_st>("logs/routes/" + log["file"].get<std::string>(), 23, 59);
            auto routeFormatter = std::make_unique<spdlog::pattern_formatter>();
            routeFormatter->add_flag<devFomat>(':');
            routeFormatter->set_pattern(format);
            fileRouteLogger->set_level(spdlog::level::info);
            fileRouteLogger->set_formatter(std::move(routeFormatter));
            routeSinks.push_back(fileRouteLogger);
            sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_st>("logs/" + log["file"].get<std::string>(), 23, 59));
        }
        auto combinedLogger = std::make_shared<spdlog::logger>("Logger", begin(sinks), end(sinks));
        spdlog::set_default_logger(combinedLogger);
        auto combinedRouteLogger = std::make_shared<spdlog::logger>("RouteLogger", begin(routeSinks), end(routeSinks));
        spdlog::register_logger(combinedRouteLogger);
        
        if (log.contains("level") && log["level"].is_string()) {
            spdlog::set_level(spdlog::level::from_str(log["level"]));
            combinedRouteLogger->set_level(spdlog::level::from_str(log["level"]));
        }
        if (log.contains("level") && log["level"].is_number()) {
            spdlog::set_level(static_cast<spdlog::level::level_enum>(log["level"]));
            combinedRouteLogger->set_level(static_cast<spdlog::level::level_enum>(log["level"]));
        }
    }
}