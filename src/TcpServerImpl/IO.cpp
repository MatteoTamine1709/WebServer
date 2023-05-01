#include "../TcpServer.h"
#include "../HttpRequestHeader.h"
#include "../Request.h"
#include "../Response.h"
#include "../utils.h"

#include <fstream>
#include <sstream>

#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>

#include <spdlog/spdlog.h>

#include <filesystem>
#include <optional>
#include <stack>

std::optional<HttpRequestHeader> TcpServer::read(TcpConnection& connection) {
    const std::string &data = connection.read();
    if (data.empty())
        return std::nullopt;
    
    HttpRequestHeader header(data);
    header.complete();

    return header;
}

std::optional<std::string> TcpServer::getCorrectPath(const std::filesystem::path &path) {
    // Handler those lines
    // /user/aaaa -> /user/[id]/index.so
    // /user/bbb/sss -> /user/[id]/[id].so
    // /user/xxx/username -> /user/[id]/username.so
    // /user/xxx/username/aaa -> /user/[id]/username/[id].so

    std::stack<std::filesystem::path> pathPart;
    std::filesystem::path newPath = std::filesystem::weakly_canonical(path);
    while (newPath.has_parent_path() &&
        newPath.string() != m_apiFolder) {
        pathPart.push(newPath.filename());
        newPath = newPath.parent_path();
    }
    // Get all children
    std::filesystem::path fallback = "";
    while (pathPart.size() > 0) {
        if (std::filesystem::is_regular_file(newPath / (pathPart.top().string() + ".so"))) {
            newPath /= (pathPart.top().string() + ".so");
            pathPart.pop();
            spdlog::debug(".so: {}", newPath.string());
            if (!std::filesystem::is_directory(newPath) && pathPart.size() > 0) {
                if (fallback.filename().string().empty())
                    return std::nullopt;
                return fallback.string();
            }
            continue;
        }
        bool found = false;
        std::string selectedPath = "";
        for (const auto &child : std::filesystem::directory_iterator(newPath)) {
            std::string childName = child.path().filename();
            if (childName == pathPart.top().string()) {
                selectedPath = childName;
                found = true;
            }
            if ((utils::startsWith(childName, "[...") && utils::endsWith(childName, "].so")) ||
                (utils::startsWith(childName, "[[...") && utils::endsWith(childName, "]].so"))) {
                fallback = newPath / childName;
            } else if (!found && utils::startsWith(childName, "[") && (utils::endsWith(childName, "]") || utils::endsWith(childName, "].so"))) {
                selectedPath = childName;
                found = true;
            }
        }
        pathPart.pop();
        spdlog::debug("Found: {}, selectedPath: {}", found, selectedPath);
        spdlog::debug("newPath: {}", newPath.string());
        spdlog::debug("fallback: {}", fallback.string());
        if (found)
            newPath /= selectedPath;
        if (pathPart.empty() && !found && std::filesystem::is_regular_file(newPath))
            return newPath.string();
        if (pathPart.empty() && found &&
            std::filesystem::is_directory(newPath) &&
            std::filesystem::is_regular_file(newPath / "index.so") &&
            std::filesystem::exists(newPath / "index.so"))
            return (newPath / "index.so").string();
        if (found && std::filesystem::is_directory(newPath) && pathPart.empty())
            for (const auto &child : std::filesystem::directory_iterator(newPath)) {
                std::string childName = child.path().filename();
                if (utils::startsWith(childName, "[[") && utils::endsWith(childName, "]].so") &&
                    std::filesystem::is_regular_file(newPath / childName))
                    return (newPath / childName).string();
                if (utils::startsWith(childName, "[[") && utils::endsWith(childName, "]]") &&
                    std::filesystem::is_directory(newPath / childName) &&
                    std::filesystem::is_regular_file(newPath / childName / "index.so")) {
                    return (newPath / childName / "index.so").string();
                }
            }
        if ((!found && !fallback.empty()) ||
            (found && std::filesystem::is_regular_file(newPath) && !pathPart.empty()) ||
            (found && std::filesystem::is_directory(newPath) && pathPart.empty()))
            return fallback.string();
    }
    if (std::filesystem::is_directory(newPath) && std::filesystem::is_regular_file(newPath / "index.so"))
        return (newPath / "index.so").string();
    if (std::filesystem::exists(newPath) && std::filesystem::is_regular_file(newPath))
        return newPath.string();
    if (!fallback.empty())
        return fallback.string();
    return std::nullopt;
}

Response TcpServer::handleRequest(HttpRequestHeader &request) {
    // spdlog::debug("Request path: {} {}", request.getMethod(), request.getPath().string());
    if (request.getMethod() == "get" && !request.isEndpoint()) {
        // spdlog::debug("Public folder: {}", m_publicFolder.string());
        request.setPath((m_publicFolder / request.getPath().relative_path().string()).string());
        // spdlog::debug("Request path: {} {}", request.getMethod(), request.getPath().string());
        if (!request.isPathValid())
            return Response(Request(request, getInstance()), getInstance());
        return handleFile(request);
        // response.complete(request);
        // return response;
    }
    auto correctPath = getCorrectPath(m_apiFolder / request.getPath().relative_path());
    if (!correctPath.has_value())
        return Response(Request(request, getInstance()), getInstance());
    spdlog::debug("Found correct path {}", correctPath.value());
    request.setPath(correctPath.value());
    // response.complete(request);
    return handleEndpoint(request);
}

Response TcpServer::handleEndpoint(HttpRequestHeader &request) {
    if (m_endpoints.find(request.getPath()) != m_endpoints.end() &&
        m_endpoints[request.getPath()].second.find(request.getMethod().data()) != m_endpoints[request.getPath()].second.end()) {
        auto &[lib, methods] = m_endpoints[request.getPath()];
        endpoint_t endpoint = methods[request.getMethod().data()];
        spdlog::debug("Found endpoint {} in library {}", request.getMethod(), request.getPath().string());
        auto req = Request(request, getInstance());
        auto res = Response(req, getInstance());
        endpoint(req, res);
        return res;
    }
    void *lib = nullptr;
    if (m_endpoints.find(request.getPath()) != m_endpoints.end()) {
        lib = m_endpoints[request.getPath()].first;
    } else {
        spdlog::debug("Loading library {}", request.getPath().string());
        lib = dlopen(request.getPath().c_str(), RTLD_LAZY);
        if (!lib) {
            SPDLOG_ERROR("Cannot open library: {}", dlerror());
            Response response(Request(request, getInstance()), getInstance());
            response.status(404).set("Content-Type", "text/html").send("Not Found");
            return response;
        }
        m_endpoints[request.getPath()] = {lib, {}};
    }
    // Load the symbols
    endpoint_t endpoint = (endpoint_t) dlsym(lib, request.getMethod().data());
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        SPDLOG_ERROR("Cannot load symbol '{}': {}", request.getMethod(), dlsym_error);
        dlclose(lib);
        Response response(Request(request, getInstance()), getInstance());
        response.status(404).set("Content-Type", "text/html").send("Not Found");
        return response;
    }

    try {
        auto req = Request(request, getInstance());
        auto res = Response(req, getInstance());
        endpoint(req, res);
        // Cache the endpoint
        m_endpoints[request.getPath()].second[request.getMethod().data()] = endpoint;
        return res;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error: [{} {}] {}", request.getMethod(), request.getRoute().string(), e.what());
        dlclose(lib);
        Response res(Request(request, getInstance()), getInstance());
        res.status(500).set("Content-Type", "text/html").send("Internal Server Error");
        return res;
    }
}

Response TcpServer::handleFile(HttpRequestHeader &request) {
    // std::ifstream file(request.getPath().c_str(), std::ios::binary);
    if (!fs::exists(request.getPath())) {
        Response response(Request(request, getInstance()), getInstance());
        response.status(404).set("Content-Type", "text/html").send("Not Found");
        return response;
    }
    // file.seekg(0, std::ios::end);
    // response.set("Content-Type", utils::getContentType(request.getPath()));
    // response.set("Content-Length", std::to_string(filesize));
    // spdlog::debug("File size: {}", filesize);
    const std::vector<std::string> accepted = utils::split(request.getHeader("Accept").value().data(), {","});

    const std::string contentType = utils::getContentType(request.getPath());
    if (std::find(accepted.begin(), accepted.end(), contentType) == accepted.end() &&
        std::find_if(accepted.begin(), accepted.end(), [](const std::string &s) { return utils::startsWith(s, "*/*"); }) == accepted.end()) {
        spdlog::debug("Not acceptable");
        Response response(Request(request, getInstance()), getInstance());
        response.sendStatus(406);
        return response;
    }
    const std::string etag = utils::makeEtag(request.getPath());
    if (request.getHeader("If-None-Match") == etag) {
        Response response(Request(request, getInstance()), getInstance());
        response.status(304)
            .set("Content-Type", utils::getContentType(request.getPath()))
            .set("Cache-Control", "max-age=0")
            .set("Content-Length", "0")
            .set("Last-Modified", utils::getLastModified(request.getPath()))
            .set("Etag", etag);
        return response;
    }
    // file.seekg(0, std::ios::beg);
    Response response(Request(request, getInstance()), getInstance());
    response.set("Etag", etag);
    response.set("Last-Modified", utils::getLastModified(request.getPath()));
    // std::stringstream ss;
    // ss << file.rdbuf();
    response.sendFile(request.getPath());
    // file.close();
    return response;
}

void TcpServer::write(TcpConnection& connection, Response& response) {
    spdlog::debug("{}", response.toReadableString());
    connection.write(response.toString());
}