#include "../TcpServer.h"
#include "../HttpRequestHeader.h"
#include "../HttpResponseHeader.h"
#include "../utils.h"

#include <iostream>
#include <thread>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>

#include <dlfcn.h>
#include <fcntl.h>

void TcpServer::handleHostConfig(nlohmann::json &host) {
    if (host.is_string()) {}
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
        m_api = api;
}

void TcpServer::handlePublicConfig(nlohmann::json &p) {
    if (p.is_string())
        m_public = p;
}

void TcpServer::handleWatchConfig(nlohmann::json &watch) {
    if (watch.is_boolean() && watch.get<bool>()) {
        const char *fifo_path = "/tmp/fifo";

        if (mkfifo(fifo_path, 0666) == -1 && errno != EEXIST) {
            std::cerr << "Failed to create named pipe." << std::endl;
            return;
        }

        m_pipeFD = open(fifo_path, O_RDONLY);
    }

}