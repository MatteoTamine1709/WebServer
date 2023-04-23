#ifndef HTTP_RESPONSE_HEADER_H
#define HTTP_RESPONSE_HEADER_H

#include <string>
#include <unordered_map>
#include <optional>

class HttpResponseHeader {
public:
    HttpResponseHeader() = default;
    HttpResponseHeader(const std::string& header);
    HttpResponseHeader(const std::string& protocol, int statusCode, const std::string& statusMessage, const std::string& body = "");
    std::string getProtocol() const;
    int getStatusCode() const;
    std::string getStatusMessage() const;
    std::optional<std::string> getHeader(const std::string& key) const;
    std::unordered_map<std::string, std::string> getHeaders() const;
    std::string getBody() const;

    void setProtocol(const std::string& protocol);
    void setStatusCode(int statusCode);
    void setStatusMessage(const std::string& statusMessage);
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);

    // Operators
    friend std::ostream& operator<<(std::ostream& os, const HttpResponseHeader& header);

    // Build response as string
    std::string buildResponse() const;
    std::string buildReadableResponse() const;
private:
    std::string m_protocol;
    int m_statusCode;
    std::string m_statusMessage;
    std::unordered_map<std::string, std::string> m_headers;
    std::string m_body;
};

#endif