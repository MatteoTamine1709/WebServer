#include <dlfcn.h>

#include <set>

#include "../TcpServer.h"
#include "../utils.h"

void TcpServer::registerMiddleware(const fs::path &middlewareFilePath) {
    if (middlewareFilePath.extension() != ".so") return;
    auto path = fs::canonical(middlewareFilePath);
    m_middlewares[path.string()] = MiddlewareInfo(path);
    auto [routes, _middlewareNames] = m_middlewares[path.string()].useFunc();
    for (const auto &route : routes) {
        std::string parsedRoute = utils::replace(route, "*", ".*");
        bool found = false;
        for (int i = 0; i < m_middlewareMatcher.size(); ++i) {
            if (m_middlewareMatcher[i].first == parsedRoute) {
                m_middlewareMatcher[i].second.push_back(path.string());
                found = true;
                break;
            }
        };
        if (!found) {
            m_middlewareMatcher.push_back(
                std::make_pair(parsedRoute, std::vector<std::string>()));
            m_middlewareMatcher.back().second.push_back(path.string());
        }
    }
    // Sort
    std::sort(m_middlewareMatcher.begin(), m_middlewareMatcher.end(),
              [](const auto &a, const auto &b) {
                  return utils::isRegexSubset(a.first, b.first);
              });
    spdlog::debug("Middleware matcher list:");
    for (const auto &middleware : m_middlewares) {
        spdlog::debug("Middleware {} {}", middleware.second.name,
                      (void *)middleware.second.func);
    }
}

void TcpServer::registerMiddlewares() {
    for (const auto &middlewareFile :
         fs::directory_iterator(m_middlewareFolder)) {
        registerMiddleware(middlewareFile.path());
    }
}

void TcpServer::runMiddleware(Request &request, Response &response,
                              std::stack<Middleware_t> &middlewareStack) {
    if (middlewareStack.empty()) return;
    auto lambda = [&]() {
        if (response.headersSent) return;
        middlewareStack.pop();
        runMiddleware(request, response, middlewareStack);
    };
    std::function<void()> next = lambda;
    middlewareStack.top()(request, response, next);
}

void TcpServer::callMiddleware(Request &request, Response &response,
                               endpoint_t endpoint) {
    std::set<Path> middlewareSet;
    std::stack<Middleware_t> middlewareStack;
    size_t i = 0;
    for (i = 0; i < m_middlewareMatcher.size(); ++i) {
        if (utils::isRegexSubset(request.path, m_middlewareMatcher[i].first)) {
            for (size_t j = 0; j < m_middlewareMatcher[i].second.size(); ++j) {
                if (middlewareSet.find(m_middlewareMatcher[i].second[j]) !=
                    middlewareSet.end())
                    continue;
                middlewareSet.insert(m_middlewareMatcher[i].second[j]);
                middlewareStack.push(
                    m_middlewares[m_middlewareMatcher[i].second[j]].func);
            }
        }
    }
    runMiddleware(request, response, middlewareStack);
    if (!response.headersSent) endpoint(request, response);
}

void TcpServer::reloadMiddleware(const std::string &path) {
    fs::path middlewareFilePath = fs::canonical(path);
    m_middlewares[middlewareFilePath.string()].reload();
}
