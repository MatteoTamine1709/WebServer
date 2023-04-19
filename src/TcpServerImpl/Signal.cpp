#include "../TcpServer.h"
#include "../HttpRequestHeader.h"
#include "../HttpResponseHeader.h"
#include "../utils.h"

#include <thread>

#include <dlfcn.h>
#include <fcntl.h>

void TcpServer::registerSignals(std::vector<std::pair<int, void (TcpServer::*)()>> signalHandlers) {
    struct sigaction action;
    action.sa_sigaction = handleSignal;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;

    for (const auto &[signal, handler] : signalHandlers) {
        if (sigaction(signal, &action, NULL) == -1) {
            SPDLOG_CRITICAL("Cannot register signal handler: {}", strerror(errno));
            exit(1);
        }
        std::thread(handler, this).detach();
    }
}

void TcpServer::handleSignal(int signum, siginfo_t *info, void *context) {
    m_signal = signum;
}

void TcpServer::hotReload() {
    while (true) {
        if (m_signal == SIGUSR1) {
            char hotReloaded_path[1024];
            ssize_t n_read = ::read(m_pipeFD, hotReloaded_path, sizeof(hotReloaded_path));
            if (n_read == -1) {
                SPDLOG_ERROR("Cannot read from pipe: {}", strerror(errno));
                continue;
            }
            hotReloaded_path[n_read] = '\0';

            std::filesystem::path path(hotReloaded_path);
            path.replace_extension(".so");
            std::string canonical = std::filesystem::canonical(path);
            if (m_endpointLibs.find(canonical) != m_endpointLibs.end())
                dlclose(m_endpointLibs[canonical]);
            void *lib = dlopen(canonical.c_str(), RTLD_LAZY);
            if (!lib) {
                SPDLOG_ERROR("Cannot open library: {}", dlerror());
                continue;
            }
            m_endpointLibs[canonical] = lib;
            SPDLOG_INFO("Hot reloaded library: {}", canonical);
        }
    }
}

void TcpServer::stopServer() {
    while (true) {
        if (m_signal == SIGINT)
            m_running = false;
    }
}