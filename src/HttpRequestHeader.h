#ifndef HTTP_REQUEST_HEADER_H
#define HTTP_REQUEST_HEADER_H

#include <string>
#include <unordered_map>
#include <optional>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class HttpRequestHeader {
public:
    HttpRequestHeader() = default;
    HttpRequestHeader(const std::string& header);
    std::string getMethod() const;
    fs::path getPath() const;
    fs::path getRoute() const;
    fs::path getUrl() const;
    std::string getProtocol() const;
    std::optional<std::string> getHeader(const std::string& key) const;
    std::unordered_map<std::string, std::string> getHeaders() const;
    std::string getRemoteAddress() const;
    std::string getRemoteUser() const;
    std::string getReferrer() const;
    std::string getUserAgent() const;
    std::string getContentType() const;
    std::string getBody() const;
    std::optional<std::string> getParameter(const std::string& key) const;
    std::unordered_map<std::string, std::string> getParameters() const;

    bool isEndpoint() const;

    void setPath(const std::string& path);
    bool isPathValid() const;
    bool isPathDirectory() const;
    void setProtocol(const std::string& protocol);
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);

    // Operators
    friend std::ostream& operator<<(std::ostream& os, const HttpRequestHeader& header);
private:
    std::string m_method;
    fs::path m_url; // http://www.example.com:8080/path/to/file.html?key=value&key2=value2#SomewhereInTheDocument
    fs::path m_route; // /path/to/file.html
    fs::path m_path; // /path/to/file.html || /path/to/endpoint
    std::string m_protocol;
    std::unordered_map<std::string, std::string> m_headers;
    std::unordered_map<std::string, std::string> m_parameters;
    std::string m_body;
};

#endif