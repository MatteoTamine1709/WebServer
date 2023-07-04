#ifndef HTTP_REQUEST_HEADER_H
#define HTTP_REQUEST_HEADER_H

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "TcpConnection.h"

namespace fs = std::filesystem;

class HttpRequestHeader {
   public:
    HttpRequestHeader() = default;
    HttpRequestHeader(const std::string_view& header);
    std::string_view getMethod() const;
    fs::path getPath() const;
    fs::path getRoute() const;
    fs::path getUrl() const;
    std::string_view getProtocol() const;
    std::optional<std::string_view> getHeader(
        const std::string_view& key) const;
    const std::unordered_map<std::string, std::string>& getHeaders() const;
    std::string_view getRemoteAddress() const;
    std::string_view getRemoteUser() const;
    std::string_view getReferrer() const;
    std::string_view getUserAgent() const;
    std::string_view getContentType() const;
    std::string_view getBody() const;
    std::optional<std::string_view> getParameter(
        const std::string_view& key) const;
    const std::unordered_map<std::string, std::string>& getParameters() const;
    const std::unordered_map<std::string, std::string>& getQueries() const;

    bool isEndpoint() const;

    void setPath(const std::string_view& path);
    void setParameters(const fs::path& apiPath);
    bool isPathValid() const;
    bool isPathDirectory() const;
    void setProtocol(const std::string_view& protocol);
    void setHeader(const std::string_view& key, const std::string_view& value);
    void setBody(const std::string_view& body);

    void complete(TcpConnection& connection);

    // Operators
    friend std::ostream& operator<<(std::ostream& os,
                                    const HttpRequestHeader& header);

   private:
    std::string m_method;
    fs::path
        m_url;  // http://www.example.com:8080/path/to/file.html?key=value&key2=value2#SomewhereInTheDocument
    fs::path m_route;  // /path/to/file.html
    fs::path m_path;   // /path/to/file.html || /path/to/endpoint
    std::string m_protocol;
    std::unordered_map<std::string, std::string> m_headers;
    std::unordered_map<std::string, std::string> m_parameters;
    std::unordered_map<std::string, std::string> m_queries;
    std::string m_body;
};

#endif