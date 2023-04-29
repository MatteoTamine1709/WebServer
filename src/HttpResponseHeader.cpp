#include "HttpResponseHeader.h"
#include "Constants.h"
#include "utils.h"
#include <sstream>
#include <array>

HttpResponseHeader::HttpResponseHeader(const std::string_view &header) {
    std::stringstream ss(header.data());
    std::string line;
    std::getline(ss, line);
    std::stringstream ssLine(line);
    std::getline(ssLine, m_protocol, ' ');
    std::string statusCode;
    std::getline(ssLine, statusCode, ' ');
    m_statusCode = std::stoi(statusCode);
    std::getline(ssLine, m_statusMessage, ' ');
    while (std::getline(ss, line)) {
        if (line == "\r")
            break;
        std::stringstream ssLine(line);
        std::string key;
        std::string value;
        std::getline(ssLine, key, ':');
        std::getline(ssLine, value, ':');
        m_headers[key] = value;
    }
    m_body = ss.str();
}

HttpResponseHeader::HttpResponseHeader(const std::string_view &protocol,
                                       int statusCode,
                                       const std::string_view &statusMessage,
                                       const std::string_view &body) :
                                            m_protocol(protocol),
                                            m_statusCode(statusCode),
                                            m_statusMessage(statusMessage) {
    if (!body.empty())
        setBody(body);
}


std::string_view HttpResponseHeader::getProtocol() const {
    return m_protocol;
}

int HttpResponseHeader::getStatusCode() const {
    return m_statusCode;
}

std::string_view HttpResponseHeader::getStatusMessage() const {
    return m_statusMessage;
}

std::optional<std::string_view> HttpResponseHeader::getHeader(const std::string_view &key) const {
    if (m_headers.find(key.data()) == m_headers.end())
        return std::nullopt;
    return m_headers.at(key.data());
}

const std::unordered_map<std::string, std::string>& HttpResponseHeader::getHeaders() const {
    return m_headers;
}

std::string_view HttpResponseHeader::getBody() const {
    return m_body;
}

void HttpResponseHeader::setProtocol(const std::string_view &protocol) {
    m_protocol = protocol;
}

void HttpResponseHeader::setStatusCode(int statusCode) {
    m_statusCode = statusCode;
}

void HttpResponseHeader::setStatusMessage(const std::string_view &statusMessage) {
    m_statusMessage = statusMessage;
}

void HttpResponseHeader::setHeader(const std::string_view &key, const std::string_view &value) {
    m_headers[key.data()] = value;
}

void HttpResponseHeader::setBody(const std::string_view &body) {
    m_body = body;
    if (m_headers.find("Content-Length") == m_headers.end())
        setHeader("Content-Length", std::to_string(body.size()));
}

void HttpResponseHeader::complete(const HttpRequestHeader &request) {
    const std::vector<std::string> &allowedMethods = {"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "CONNECT", "TRACE"};
    setHeader("Server", "MyCustomServer/1.0");
    setHeader("Date", utils::getDateTime());
    if (request.getMethod() == "options" && !getHeader("Allow").has_value())
        setHeader("Allow", utils::join(allowedMethods, ", "));
    if (request.getHeader("Connection").has_value() && !getHeader("Connection").has_value())
        setHeader("Connection", request.getHeader("Connection").value());
    if (request.getHeader("Cache-Control").has_value() && !getHeader("Cache-Control").has_value())
        setHeader("Cache-Control", request.getHeader("Cache-Control").value());
}

// Operators
std::ostream &operator<<(std::ostream &os, const HttpResponseHeader &header) {
    os << header.m_protocol << " " << header.m_statusCode << " " << header.m_statusMessage << "\r\n";
    for (auto &header : header.m_headers) {
        os << header.first << ": " << header.second << "\r\n";
    }
    os << "\r\n";
    os << header.m_body;
    return os;
}

// Build response as string
std::string HttpResponseHeader::buildResponse() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

std::string HttpResponseHeader::buildReadableResponse() const {
    std::stringstream ss;
    ss << "Protocol: " << m_protocol << "\r\n";
    ss << "Status Code: " << m_statusCode << "\r\n";
    ss << "Status Message: " << m_statusMessage << "\r\n";
    for (auto &header : m_headers)
        ss << header.first << ": " << header.second << "\r\n";
    ss << "\r\n";
    if (m_body.size() > 0 && m_body.size() < ONE_KILOBYTE) {
        ss << m_body;
    } else if (m_body.size() > 0) {
        ss << "Body size: " << m_body.size() << " bytes";
    }
    return ss.str();
}