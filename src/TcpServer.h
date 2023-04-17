#ifndef HTTP_TCP_SERVER_H
#define HTTP_TCP_SERVER_H

#include <memory>
#include <string>
#include <vector>

#include "TcpConnection.h"
#include "HttpRequestHeader.h"
#include "HttpResponseHeader.h"

class TcpServer {
public:
    static TcpServer& getInstance(const std::string& address, const std::string& port);

    TcpServer(const TcpServer&) = delete; // Copy constructor
    TcpServer(TcpServer&&) = delete; // Move constructor
    TcpServer& operator=(const TcpServer&) = delete; // Copy assignment operator
    TcpServer& operator=(TcpServer&&) = delete; // Move assignment operator

    ~TcpServer();

    void run();

private:
    TcpServer(const std::string& address, const std::string& port);
    void accept();
    std::optional<HttpRequestHeader> read(TcpConnection& connection);
    HttpResponseHeader &completeResponse(HttpResponseHeader &response, const HttpRequestHeader &request);
    HttpResponseHeader handleRequest(const HttpRequestHeader& request);
    HttpResponseHeader handleEndpoint(const HttpRequestHeader& request);
    HttpResponseHeader handleFile(const HttpRequestHeader& request);
    void write(TcpConnection& connection, const HttpResponseHeader& response);

    static void handleSignal(int signal);

    static std::unique_ptr<TcpServer> m_instance;

    int m_socket;
    static bool m_running;

    typedef std::string Endpoint;
    typedef std::string Method;
    std::unordered_map<Endpoint, void *> m_endpointLibs;
    typedef HttpResponseHeader (*endpoint_t)(HttpRequestHeader);
    std::unordered_map<Endpoint, std::unordered_map<Method, endpoint_t>> m_endpointHandlers;
};

#endif
