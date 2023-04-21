#include "../TcpServer.h"
#include "../HttpRequestHeader.h"
#include "../HttpResponseHeader.h"
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

HttpRequestHeader &TcpServer::completeRequest(HttpRequestHeader &request) {
    if (request.getProtocol().empty())
        request.setProtocol("HTTP/1.1");
    // request.setUrl("m_host + request.getPath()");

    return request;
} 

std::optional<HttpRequestHeader> TcpServer::read(TcpConnection& connection) {
    const std::string &data = connection.read();
    if (data.empty())
        return std::nullopt;
    
    HttpRequestHeader header(data);
    completeRequest(header);

    return header;
}

HttpResponseHeader &TcpServer::completeResponse(HttpResponseHeader &response, const HttpRequestHeader &request) {
    const std::vector<std::string> &allowedMethods = {"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "CONNECT", "TRACE"};
    response.setHeader("Server", "MyCustomServer/1.0");
    response.setHeader("Date", utils::getDateTime());
    if (request.getMethod() == "options" && !response.getHeader("Allow").has_value())
        response.setHeader("Allow", utils::join(allowedMethods, ", "));
    if (request.getHeader("Connection").has_value() && !response.getHeader("Connection").has_value())
        response.setHeader("Connection", request.getHeader("Connection").value());
    if (request.getHeader("Cache-Control").has_value() && !response.getHeader("Cache-Control").has_value())
        response.setHeader("Cache-Control", request.getHeader("Cache-Control").value());
    return response;
}

std::optional<std::string> TcpServer::getCorrectPathOrEmpty(const std::filesystem::path &path) {
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
                return fallback;
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
            if (utils::startsWith(childName, "[...") && utils::endsWith(childName, "].so")) {
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
            return newPath;
        if (pathPart.empty() && found &&
            std::filesystem::is_directory(newPath) &&
            std::filesystem::is_regular_file(newPath / "index.so"))
            return newPath / "index.so";

        if ((!found && !fallback.empty()) ||
            (found && std::filesystem::is_regular_file(newPath) && !pathPart.empty()) ||
            (found && std::filesystem::is_directory(newPath) && pathPart.empty()))
            return fallback;
    }
    if (std::filesystem::is_directory(newPath))
        return newPath / "index.so";
    if (std::filesystem::exists(newPath))
        return newPath;
    return std::nullopt;
}

HttpResponseHeader TcpServer::handleRequest(HttpRequestHeader &request) {
    // TODO: Handle parematerized paths
    // TODO: Handle request parameters
    spdlog::debug("Request path: {}", request.getPath());
    if (request.getMethod() == "get" && !utils::getExtension(request.getRoute()).empty()) {
        request.setPath(m_publicFolder + request.getPath());
        if (!request.isPathValid())
            return HttpResponseHeader("HTTP/1.1", "404", "Not Found", "Not Found");
        HttpResponseHeader response = handleFile(request);
        completeResponse(response, request);
        return response;
    }
    auto correctPath = getCorrectPathOrEmpty(m_apiFolder + request.getPath());
    if (!correctPath.has_value())
        return HttpResponseHeader("HTTP/1.1", "404", "Not Found", "Not Found");
    spdlog::debug("Found correct path {}", correctPath.value());
    request.setPath(correctPath.value());
    HttpResponseHeader response = handleEndpoint(request);
    completeResponse(response, request);
    return response;
}

HttpResponseHeader TcpServer::handleEndpoint(HttpRequestHeader &request) {
    if (m_endpoints.find(request.getPath()) != m_endpoints.end() &&
        m_endpoints[request.getPath()].second.find(request.getMethod()) != m_endpoints[request.getPath()].second.end()) {
        auto &[lib, methods] = m_endpoints[request.getPath()];
        endpoint_t endpoint = methods[request.getMethod()];
        spdlog::debug("Found endpoint {} in library {}", request.getMethod(), request.getPath());
        return endpoint(request);
    }
    void *lib = nullptr;
    if (m_endpoints.find(request.getPath()) != m_endpoints.end()) {
        lib = m_endpoints[request.getPath()].first;
    } else {
        spdlog::debug("Loading library {}", request.getPath());
        lib = dlopen(request.getPath().c_str(), RTLD_LAZY);
        if (!lib) {
            SPDLOG_ERROR("Cannot open library: {}", dlerror());
            HttpResponseHeader response("HTTP/1.1", "404", "Not Found", "Not Found");
            response.setHeader("Content-Type", "text/html");
            return response;
        }
        m_endpoints[request.getPath()] = {lib, {}};
    }
    // Load the symbols
    endpoint_t endpoint = (endpoint_t) dlsym(lib, request.getMethod().c_str());
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        SPDLOG_ERROR("Cannot load symbol '{}': {}", request.getMethod(), dlsym_error);
        dlclose(lib);
        HttpResponseHeader response("HTTP/1.1", "404", "Not Found", "Not Found");
        response.setHeader("Content-Type", "text/html");
        return response;
    }

    HttpResponseHeader response;
    try {
        response = endpoint(request);
        // Cache the endpoint
        m_endpoints[request.getPath()].second[request.getMethod()] = endpoint;
        return response;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error: [{} {}] {}", request.getMethod(), request.getRoute(), e.what());
        dlclose(lib);
        HttpResponseHeader response("HTTP/1.1", "500", "Internal Server Error", "Internal Server Error");
        response.setHeader("Content-Type", "text/html");
        return response;
    }
}

HttpResponseHeader TcpServer::handleFile(HttpRequestHeader &request) {
    std::ifstream file(request.getPath().c_str(), std::ios::binary);
    if (!file.is_open()) {
        HttpResponseHeader response("HTTP/1.1", "404", "Not Found", "Not Found");
        response.setHeader("Content-Type", "text/html");
        return response;
    }

    file.seekg(0, std::ios::end);
    const std::streamsize filesize = file.tellg();
    file.seekg(0, std::ios::beg);
    HttpResponseHeader response("HTTP/1.1", "200", "OK", "OK");
    response.setHeader("Content-Type", utils::getContentType(request.getPath()));
    std::stringstream ss;
    ss << file.rdbuf();
    response.setBody(ss.str());
    return response;
}

void TcpServer::write(TcpConnection& connection, const HttpResponseHeader& response) {
    // std::cout << response.buildReadableResponse() << std::endl;
    connection.write(response.buildResponse());
    // std::ifstream file("assets/videos/Studio_Project.mp4", std::ios::binary);
    // file.seekg(0, std::ios::end);
    // const std::streamsize filesize = file.tellg();
    // file.seekg(0, std::ios::beg);
    // std::ostringstream header;
    // header << "HTTP/1.1 200 OK\r\n"
    //         << "Content-Type: video/mp4\r\n"
    //         << "Content-Length: " << 4 * 1024 * 1024 << "\r\n"
    //         << "Connection: close\r\n"
    //         << "\r\n";
    // // std::cout << content << std::endl;

    // const std::string header_str = header.str();
    // connection.write(header_str);
    // std::vector<char> buffer(1 * 1024 * 1024); // 1MB
    // // while (file.good()) {
    //     file.read(buffer.data(), buffer.size());
    //     const std::streamsize count = file.gcount();
    //     // if (count == 0)
    //     //     break;
    //     connection.write(std::string(buffer.data(), count));
    // }
}