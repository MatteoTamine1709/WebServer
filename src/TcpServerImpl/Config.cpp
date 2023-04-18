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
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

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
            return spdlog::error("Failed to create FIFO: {}", strerror(errno));
        spdlog::info("Created FIFO at {}", fifo_path);
        spdlog::info("Waiting for hot reloader to connect...");
        m_pipeFD = open(fifo_path, O_RDONLY);
    }
}

void TcpServer::handleLogConfig(nlohmann::json &log) {
    if (log.is_string())
        spdlog::set_level(spdlog::level::from_str(log));
    
    if (log.is_number())
        spdlog::set_level(static_cast<spdlog::level::level_enum>(log));

    if (log.is_object()) {
        if (log.contains("level"))
            spdlog::set_level(spdlog::level::from_str(log["level"]));
        std::vector<spdlog::sink_ptr> sinks;
        if (!log.contains("no-stdout") || !log["no-stdout"].is_boolean() || !log["no-stdout"].get<bool>())
            sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
        if (log.contains("file") && log["file"].is_string())
            sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_st>("logs/" + log["file"].get<std::string>(), 23, 59));
        auto combined_logger = std::make_shared<spdlog::logger>("Logger", begin(sinks), end(sinks));
        spdlog::set_default_logger(combined_logger);
        if (log.contains("format") && log["format"].is_string()) {
            std::unordered_map<std::string, std::string> format_map = {
                {"combined", "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v"},
                {"common", "%h %l %u %t \"%r\" %s %b"},
                {"dev", "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v"},
                {"short", "%^[%l] [%s:%#] %v%$"},
                {"tiny", "%^[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v%$"}
            };

            if (format_map.find(log["format"]) != format_map.end())
                spdlog::set_pattern(format_map[log["format"]]);
            else
                spdlog::set_pattern(log["format"]);
        }
    }
}