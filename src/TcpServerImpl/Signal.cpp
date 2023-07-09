#include <dlfcn.h>
#include <fcntl.h>

#include <thread>

#include "../HttpRequestHeader.h"
#include "../TcpServer.h"
#include "../utils.h"

std::function<void(int, siginfo_t *, void *)> handler;
void callHandler(int signum, siginfo_t *info, void *context) {
    handler(signum, info, context);
}

void TcpServer::registerSignals(std::vector<int> signals) {
    struct sigaction action;
    handler = std::bind(&TcpServer::handleSignal, this, std::placeholders::_1,
                        std::placeholders::_2, std::placeholders::_3);
    action.sa_sigaction = callHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;

    for (const auto &signal : signals) {
        if (sigaction(signal, &action, NULL) == -1) {
            SPDLOG_CRITICAL("Cannot register signal handler: {}",
                            strerror(errno));
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
    if (!m_watch) return;
    static pid_t hotReloaderPID = -1;
    if (signum == SIGUSR1 && hotReloaderPID == -1) {
        SPDLOG_INFO("Connecting to hot reloader...");
        size_t n_read =
            ::read(m_pipeWatchFD, &hotReloaderPID, sizeof(hotReloaderPID));
        if (n_read == -1) {
            SPDLOG_ERROR("Cannot connect to hot reloader!");
            return SPDLOG_ERROR("Cannot read from pipe: {}", strerror(errno));
        }
        spdlog::debug("Hot reloader PID: {}", hotReloaderPID);
        if (::write(m_pipeWatchFD, m_apiFolder.c_str(),
                    m_apiFolder.string().size()) == -1) {
            SPDLOG_ERROR("Cannot connect to hot reloader!");
            return SPDLOG_ERROR("Cannot write to pipe: {}", strerror(errno));
        }
        kill(hotReloaderPID, SIGUSR1);
        return;
    }
    if (signum == SIGUSR1 && !m_isHotReloaderConnected) {
        m_isHotReloaderConnected = true;
        return SPDLOG_INFO("Hot reloader connected");
    }
    if (signum == SIGUSR1 && m_isHotReloaderConnected) {
        m_isHotReloaderConnected = false;
        hotReloaderPID = -1;
        return SPDLOG_WARN("Hot reloader disconnected");
    }

    if (m_signal == SIGUSR2 && m_isHotReloaderConnected) {
        char hotReloaded_path[1024];
        ssize_t n_read =
            ::read(m_pipeWatchFD, hotReloaded_path, sizeof(hotReloaded_path));
        if (n_read == -1) {
            SPDLOG_ERROR("Cannot read from pipe: {}", strerror(errno));
            return;
        }
        hotReloaded_path[n_read] = '\0';

        fs::path path(hotReloaded_path);
        path.replace_extension(".so");
        std::string canonical = fs::weakly_canonical(path).string();
        if (canonical.find(m_middlewareFolder.string()) != std::string::npos) {
            return reloadMiddleware(canonical);
        }
        if (m_endpoints.find(canonical) != m_endpoints.end()) {
            SPDLOG_INFO("Hot reloaded library: {}", canonical);
            dlclose(m_endpoints[canonical].first);
            m_endpoints[canonical].second.clear();
        } else {
            SPDLOG_INFO("Loaded library: {}", canonical);
        }
        void *lib = dlopen(canonical.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!lib) {
            SPDLOG_ERROR("Cannot open library: {}", dlerror());
            return;
        }
        m_endpoints[canonical] = {lib, {}};
    }
}