#ifndef HTTP_REQUEST_HEADER_H
#define HTTP_REQUEST_HEADER_H

#include <string>
#include <unordered_map>
#include <optional>

class HttpRequestHeader {
public:
    HttpRequestHeader() = default;
    HttpRequestHeader(const std::string& header);
    std::string getMethod() const;
    std::string getPath() const;
    std::string getProtocol() const;
    std::optional<std::string> getHeader(const std::string& key) const;
    std::string getBody() const;

    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);

    // Operators
    friend std::ostream& operator<<(std::ostream& os, const HttpRequestHeader& header);
private:
    std::string m_method;
    std::string m_path;
    std::string m_protocol;
    std::unordered_map<std::string, std::string> m_headers;
    std::string m_body;
};

#endif