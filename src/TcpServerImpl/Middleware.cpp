#include "../TcpServer.h"
#include "../utils.h"
#include <dlfcn.h>
#include <set>

void TcpServer::registerMiddlewares() {
    for (const auto &middlewareFile : fs::directory_iterator(m_middlewareFolder)) {
        if (middlewareFile.path().extension() != ".so")
            continue;
        void *handle = dlopen(middlewareFile.path().c_str(), RTLD_NOW);
        if (!handle) {
            spdlog::error("Failed to load middleware {}: {}", middlewareFile.path().filename().string(), dlerror());
            continue;
        }
        m_middlewareLibraries[fs::canonical(middlewareFile.path()).string()] = handle;
        auto useFunc = (const std::pair<std::vector<std::string>, std::vector<std::string>>(*)())(dlsym(handle, "use"));
        auto nameFunc = (const std::string (*)())(dlsym(handle, "name"));
        if (!useFunc || !nameFunc) {
            spdlog::error("Failed to load middleware {}: {}", middlewareFile.path().filename().string(), dlerror());
            continue;
        }
        spdlog::debug("Registering middleware {} for routes {}", nameFunc(), utils::join(useFunc().first, ", "));
        auto middlewareFunc = (Middleware_t)(dlsym(handle, nameFunc().c_str()));
        if (!middlewareFunc) {
            spdlog::error("Failed to load middleware {}: {}", middlewareFile.path().filename().string(), dlerror());
            continue;
        }
        auto [routes, middlewareNames] = useFunc();
        auto name = nameFunc();

        for (const auto &route : routes)
            m_middlewareMatcher.push_back({utils::replace(route, "*", ".*"), middlewareNames});
        m_middlewares[name] = middlewareFunc;
        m_middlewarePathToName[fs::canonical(middlewareFile.path()).string()] = name;
    }
    std::sort(m_middlewareMatcher.begin(), m_middlewareMatcher.end(),
        [](const std::pair<std::string, std::vector<std::string>> &a, const std::pair<std::string, std::vector<std::string>> &b) {
            return utils::isRegexSubset(a.first, b.first);
        });
}

void TcpServer::runMiddleware(const Request &request, Response &response, std::stack<Middleware_t> &middlewareStack) {
    if (middlewareStack.empty())
        return;
    auto lambda = [&]() {
        if (response.headersSent)
            return;
        middlewareStack.pop();
        runMiddleware(request, response, middlewareStack);
    };
    std::function<void()> next = lambda;
    middlewareStack.top()(request, response, next);
    middlewareStack.pop();
}

void TcpServer::callMiddleware(const Request &request, Response &response, endpoint_t endpoint) {
    std::set<Middleware_t> middlewareSet;
    std::stack<Middleware_t> middlewareStack;
    for (const auto &[regex, middlewareNames] : m_middlewareMatcher) {
        if (!std::regex_match(request.path, std::regex(regex)))
            continue;
        for (const auto &middlewareName : middlewareNames) {
            auto middleware = m_middlewares.find(middlewareName);
            if (middleware == m_middlewares.end()) {
                spdlog::error("Middleware {} not found", middlewareName);
                continue;
            }
            if (middlewareSet.find(middleware->second) != middlewareSet.end())
                continue;
            middlewareSet.insert(middleware->second);
            middlewareStack.push(middleware->second);
            spdlog::debug("Middleware {} {}", regex, (void *)middleware->second);
        }
    }
    spdlog::debug("Number of middlewares: {}", middlewareStack.size());
    runMiddleware(request, response, middlewareStack);
    if (!response.headersSent)
        endpoint(request, response);
}

void TcpServer::reloadMiddleware(const std::string &path) {
    std::string oldName = m_middlewarePathToName[path];
    m_middlewares.erase(oldName);
    m_middlewareMatcher.erase(std::remove_if(m_middlewareMatcher.begin(), m_middlewareMatcher.end(),
        [&](const std::pair<std::string, std::vector<std::string>> &a) {
            return std::find(a.second.begin(), a.second.end(), oldName) != a.second.end();
        }), m_middlewareMatcher.end());
    dlclose(m_middlewareLibraries[path]);
    m_middlewareLibraries.erase(path);

    void *handle = dlopen(path.c_str(), RTLD_NOW);
    if (!handle) {
        spdlog::error("Failed to load middleware {}: {}", path, dlerror());
        return;
    }
    m_middlewareLibraries[fs::canonical(path).string()] = handle;
    auto useFunc = (const std::pair<std::vector<std::string>, std::vector<std::string>>(*)())(dlsym(handle, "use"));
    auto nameFunc = (const std::string (*)())(dlsym(handle, "name"));
    if (!useFunc || !nameFunc) {
        spdlog::error("Failed to load middleware {}: {}", path, dlerror());
        return;
    }
    spdlog::debug("Registering middleware {} for routes {}", nameFunc(), utils::join(useFunc().first, ", "));
    auto middlewareFunc = (Middleware_t)(dlsym(handle, nameFunc().c_str()));
    if (!middlewareFunc) {
        spdlog::error("Failed to load middleware {}: {}", path, dlerror());
        return;
    }
    auto [routes, middlewareNames] = useFunc();
    auto name = nameFunc();
    
    for (const auto &route : routes)
        m_middlewareMatcher.push_back({utils::replace(route, "*", ".*"), middlewareNames});
    m_middlewares[name] = middlewareFunc;
    m_middlewarePathToName[fs::canonical(path).string()] = name;
}
