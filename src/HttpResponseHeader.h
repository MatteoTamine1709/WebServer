#ifndef HTTP_RESPONSE_HEADER_H
#define HTTP_RESPONSE_HEADER_H

#include <string>
#include <unordered_map>
#include <optional>
#include "HttpRequestHeader.h"

class HttpResponseHeader {
public:
    HttpResponseHeader() = default;
    HttpResponseHeader(const std::string_view& header);
    HttpResponseHeader(const std::string_view& protocol, int statusCode, const std::string_view& statusMessage, const std::string_view& body = "");
    std::string_view getProtocol() const;
    int getStatusCode() const;
    std::string_view getStatusMessage() const;
    std::optional<std::string_view> getHeader(const std::string_view& key) const;
    const std::unordered_map<std::string, std::string>& getHeaders() const;
    std::string_view getBody() const;

    void setProtocol(const std::string_view& protocol);
    void setStatusCode(int statusCode);
    void setStatusMessage(const std::string_view& statusMessage);
    void setHeader(const std::string_view& key, const std::string_view& value);
    void setBody(const std::string_view& body);

    void complete(const HttpRequestHeader& requestHeader);

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