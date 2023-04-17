#include "TcpServer.h"
#include "HttpRequestHeader.h"
#include "HttpResponseHeader.h"
#include "utils.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <netdb.h>
#include <cstring>
#include "signal.h"

#include <fstream>
#include <sstream>

#include <dlfcn.h>

std::unique_ptr<TcpServer> TcpServer::m_instance = nullptr;
bool TcpServer::m_running = true;

TcpServer& TcpServer::getInstance(const std::string& address, const std::string& port) {
    if (!m_instance) {
        m_instance = std::unique_ptr<TcpServer>(new TcpServer(address, port));
    }
    return *m_instance;
}

TcpServer::TcpServer(const std::string& address, const std::string& port) {
    // Better handling when quitting the server?
    // signal(SIGINT, TcpServer::handleSignal);
    addrinfo hints;
    addrinfo* result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int status = getaddrinfo(address.c_str(), port.c_str(), &hints, &result);
    if (status != 0) {
        throw std::runtime_error("getaddrinfo() failed");
    }
    m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (m_socket == -1) {
        freeaddrinfo(result);
        throw std::runtime_error("socket() failed");
    }
    int a = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(int));
    status = bind(m_socket, result->ai_addr, result->ai_addrlen);
    if (status == -1) {
        freeaddrinfo(result);
        close(m_socket);
        throw std::runtime_error("bind() failed");
    }
    freeaddrinfo(result);
    status = listen(m_socket, 10);
    if (status == -1) {
        close(m_socket);
        throw std::runtime_error("listen() failed");
    }
}

TcpServer::~TcpServer() {
    for (auto& lib : m_endpointLibs)
        dlclose(lib.second);
    close(m_socket);
}

void TcpServer::run() {
    utils::initializeMimeMap();
    while (m_running)
        accept();
}

void TcpServer::accept() {
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    int clientSocket = ::accept(m_socket, (sockaddr*)&client, &clientSize);
    if (clientSocket == -1)
        throw std::runtime_error("accept() failed");
    std::cout << "New connection" << std::endl;

    std::thread t([clientSocket, this](){
        TcpConnection connection(clientSocket);
        while (connection.isOpen()) {
            auto requestHeader = read(connection);
            if (!requestHeader)
                continue;
            HttpResponseHeader responseHeader = handleRequest(requestHeader.value());
            write(connection, responseHeader);
            // Unsure if this is the best way to handle keep-alive connections
            if (requestHeader.value().getHeader("Connection").has_value() && requestHeader.value().getHeader("Connection").value() != "keep-alive")
                break;
        }
        close(clientSocket);
    });
    t.detach();
}

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

HttpResponseHeader TcpServer::handleRequest(const HttpRequestHeader &request) {

    if (request.getMethod() == "get" && !utils::getExtension(request.getPath()).empty()) {
        HttpResponseHeader response = handleFile(request);
        completeResponse(response, request);
        return response;
    }
    HttpResponseHeader response = handleEndpoint(request);
    completeResponse(response, request);
    return response;
}

HttpResponseHeader TcpServer::handleEndpoint(const HttpRequestHeader &request) {
    // Get environment path and append the requested path
    std::string newPath = "endpoints" + request.getPath() + "libindex.so";
    // TODO: Handle parematerized paths
    // TODO: Handle request parameter
    std::cout << "New path: " << newPath << std::endl;
    if (m_endpointHandlers.find(request.getPath()) != m_endpointHandlers.end() &&
        m_endpointHandlers[request.getPath()].find(request.getMethod()) != m_endpointHandlers[request.getPath()].end()) {
        endpoint_t endpoint = m_endpointHandlers[request.getPath()][request.getMethod()];
        return endpoint(request);
    }
    void* lib = nullptr;
    if (m_endpointLibs.find(request.getPath()) != m_endpointLibs.end()) {
        lib = m_endpointLibs[request.getPath()];
    } else {
        lib = dlopen(newPath.c_str(), RTLD_LAZY);
        if (!lib) {
            std::cerr << "Cannot open library: " << dlerror() << '\n';
            HttpResponseHeader response("HTTP/1.1", "404", "Not Found", "Not Found");
            response.setHeader("Content-Type", "text/html");
            return response;
        }
        m_endpointLibs[request.getPath()] = lib;
    }
    // Load the symbols
    endpoint_t endpoint = (endpoint_t) dlsym(lib, request.getMethod().c_str());
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol '" << request.getMethod() << "': " << dlsym_error << '\n';
        dlclose(lib);
        HttpResponseHeader response("HTTP/1.1", "404", "Not Found", "Not Found");
        response.setHeader("Content-Type", "text/html");
        return response;
    }

    HttpResponseHeader response;
    try {
        response = endpoint(request);
        // Cache the endpoint
        m_endpointHandlers[request.getPath()][request.getMethod()] = endpoint;
        return response;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        dlclose(lib);
        HttpResponseHeader response("HTTP/1.1", "500", "Internal Server Error", "Internal Server Error");
        response.setHeader("Content-Type", "text/html");
        return response;
    }
}

HttpResponseHeader TcpServer::handleFile(const HttpRequestHeader &response) {
    std::string path = "public" + response.getPath();
    std::cout << path << std::endl;
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        HttpResponseHeader response("HTTP/1.1", "404", "Not Found", "Not Found");
        response.setHeader("Content-Type", "text/html");
        return response;
    }

    file.seekg(0, std::ios::end);
    const std::streamsize filesize = file.tellg();
    file.seekg(0, std::ios::beg);
    HttpResponseHeader reponse("HTTP/1.1", "200", "OK", "OK");
    reponse.setHeader("Content-Type", utils::getContentType(path));
    std::stringstream ss;
    ss << file.rdbuf();
    reponse.setBody(ss.str());
    return reponse;
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

void TcpServer::handleSignal(int signal) {
    m_running = false;
}