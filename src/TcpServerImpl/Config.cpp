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
        m_pipeFD = open(fifo_path, O_RDONLY);
    }
}

void TcpServer::handleDebugConfig(nlohmann::json &debug) {
    if (debug.is_boolean() && debug.get<bool>())
        spdlog::set_level(spdlog::level::debug);
}