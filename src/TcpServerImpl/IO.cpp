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

std::optional<HttpRequestHeader> TcpServer::read(TcpConnection& connection) {
    const std::string &data = connection.read();
    if (data.empty())
        return std::nullopt;
    
    HttpRequestHeader header(data);

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

HttpResponseHeader TcpServer::handleRequest(HttpRequestHeader &request) {
    // TODO: Handle parematerized paths
    // TODO: Handle request parameters
    if (request.getMethod() == "get" && !utils::getExtension(request.getPath()).empty()) {
        request.setPath(m_public + request.getPath());
        if (!request.isPathValid())
            return HttpResponseHeader("HTTP/1.1", "404", "Not Found", "Not Found");
        spdlog::debug("Canonical path: {}", request.getCanonicalPath());
        HttpResponseHeader response = handleFile(request);
        completeResponse(response, request);
        return response;
    }
    request.setPath(m_api + request.getPath());
    request.setPath(request.getCanonicalPath() + "/index.so");
    if (!request.isPathValid())
        return HttpResponseHeader("HTTP/1.1", "404", "Not Found", "Not Found");
    spdlog::debug("Canonical path: {}", request.getCanonicalPath());
    HttpResponseHeader response = handleEndpoint(request);
    completeResponse(response, request);
    return response;
}

HttpResponseHeader TcpServer::handleEndpoint(HttpRequestHeader &request) {
    if (m_endpointHandlers.find(request.getCanonicalPath()) != m_endpointHandlers.end() &&
        m_endpointHandlers[request.getCanonicalPath()].find(request.getMethod()) != m_endpointHandlers[request.getCanonicalPath()].end()) {
        endpoint_t endpoint = m_endpointHandlers[request.getCanonicalPath()][request.getMethod()];
        return endpoint(request);
    }
    void *lib = nullptr;
    if (m_endpointLibs.find(request.getCanonicalPath()) != m_endpointLibs.end()) {
        lib = m_endpointLibs[request.getCanonicalPath()];
    } else {
        lib = dlopen(request.getCanonicalPath().c_str(), RTLD_LAZY);
        if (!lib) {
            spdlog::error("Cannot open library: {}", dlerror());
            HttpResponseHeader response("HTTP/1.1", "404", "Not Found", "Not Found");
            response.setHeader("Content-Type", "text/html");
            return response;
        }
        m_endpointLibs[request.getCanonicalPath()] = lib;
    }
    // Load the symbols
    endpoint_t endpoint = (endpoint_t) dlsym(lib, request.getMethod().c_str());
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        spdlog::error("Cannot load symbol '{}': {}", request.getMethod(), dlsym_error);
        dlclose(lib);
        HttpResponseHeader response("HTTP/1.1", "404", "Not Found", "Not Found");
        response.setHeader("Content-Type", "text/html");
        return response;
    }

    HttpResponseHeader response;
    try {
        response = endpoint(request);
        // Cache the endpoint
        m_endpointHandlers[request.getCanonicalPath()][request.getMethod()] = endpoint;
        return response;
    } catch (const std::exception& e) {
        spdlog::error("Error: {}", e.what());
        dlclose(lib);
        HttpResponseHeader response("HTTP/1.1", "500", "Internal Server Error", "Internal Server Error");
        response.setHeader("Content-Type", "text/html");
        return response;
    }
}

HttpResponseHeader TcpServer::handleFile(HttpRequestHeader &request) {
    std::ifstream file(request.getCanonicalPath().c_str(), std::ios::binary);
    if (!file.is_open()) {
        HttpResponseHeader response("HTTP/1.1", "404", "Not Found", "Not Found");
        response.setHeader("Content-Type", "text/html");
        return response;
    }

    file.seekg(0, std::ios::end);
    const std::streamsize filesize = file.tellg();
    file.seekg(0, std::ios::beg);
    HttpResponseHeader response("HTTP/1.1", "200", "OK", "OK");
    response.setHeader("Content-Type", utils::getContentType(request.getCanonicalPath()));
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