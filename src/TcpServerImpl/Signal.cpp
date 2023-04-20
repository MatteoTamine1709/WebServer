#include "../TcpServer.h"
#include "../HttpRequestHeader.h"
#include "../HttpResponseHeader.h"
#include "../utils.h"

#include <thread>

#include <dlfcn.h>
#include <fcntl.h>

std::function<void(int, siginfo_t *, void *)> handler;
void callHandler(int signum, siginfo_t *info, void *context) {
    handler(signum, info, context);
}

void TcpServer::registerSignals(std::vector<int> signals) {
    struct sigaction action;
    handler = std::bind(&TcpServer::handleSignal, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    action.sa_sigaction = callHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;

    for (const auto &signal : signals) {
        if (sigaction(signal, &action, NULL) == -1) {
            SPDLOG_CRITICAL("Cannot register signal handler: {}", strerror(errno));
            exit(1);
        }
    }
}

void TcpServer::handleSignal(int signum, siginfo_t *info, void *context) {
    m_signal = signum;

    if (signum == SIGINT || signum == SIGTERM) {
        m_running = false;
        return;
    }
    if (!m_watch)
        return;
    static bool hotReloaderConnected = false;
    if (signum == SIGUSR2 && !hotReloaderConnected) {
        hotReloaderConnected = true;
        return SPDLOG_INFO("Hot reloader connected");
    }
    if (signum == SIGUSR2 && hotReloaderConnected) {
        hotReloaderConnected = false;
        return SPDLOG_WARN("Hot reloader disconnected");
    }

    std::string canonical = "";
    if ((m_signal == SIGUSR1 || m_signal == SIGUSR2)) {
        char hotReloaded_path[1024];
        ssize_t n_read = ::read(m_pipeFD, hotReloaded_path, sizeof(hotReloaded_path));
        if (n_read == -1) {
            SPDLOG_ERROR("Cannot read from pipe: {}", strerror(errno));
            return;
        }
        hotReloaded_path[n_read] = '\0';

        std::filesystem::path path(hotReloaded_path);
        path.replace_extension(".so");
        canonical = std::filesystem::canonical(path);
        if (m_endpoints.find(canonical) != m_endpoints.end()) {
            SPDLOG_INFO("Hot reloaded library: {}", canonical);
            dlclose(m_endpoints[canonical].first);
            m_endpoints[canonical].second.clear();
        } else {
            SPDLOG_INFO("Loaded library: {}", canonical);
        }
        void *lib = dlopen(canonical.c_str(), RTLD_LAZY);
        if (!lib) {
            SPDLOG_ERROR("Cannot open library: {}", dlerror());
            return;
        }
        m_endpoints[canonical] = {lib, {}};
    }
}