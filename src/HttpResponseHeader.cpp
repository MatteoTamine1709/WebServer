#include "HttpResponseHeader.h"
#include "Constants.h"

#include <sstream>

HttpResponseHeader::HttpResponseHeader(const std::string &header) {
    std::stringstream ss(header);
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

HttpResponseHeader::HttpResponseHeader(const std::string &protocol,
                                       int statusCode,
                                       const std::string &statusMessage,
                                       const std::string &body) :
                                            m_protocol(protocol),
                                            m_statusCode(statusCode),
                                            m_statusMessage(statusMessage) {
    setBody(body);
}


std::string HttpResponseHeader::getProtocol() const {
    return m_protocol;
}

int HttpResponseHeader::getStatusCode() const {
    return m_statusCode;
}

std::string HttpResponseHeader::getStatusMessage() const {
    return m_statusMessage;
}

std::optional<std::string> HttpResponseHeader::getHeader(const std::string &key) const {
    if (m_headers.find(key) == m_headers.end())
        return std::nullopt;
    return m_headers.at(key);
}

std::unordered_map<std::string, std::string> HttpResponseHeader::getHeaders() const {
    return m_headers;
}

std::string HttpResponseHeader::getBody() const {
    return m_body;
}

void HttpResponseHeader::setProtocol(const std::string &protocol) {
    m_protocol = protocol;
}

void HttpResponseHeader::setStatusCode(int statusCode) {
    m_statusCode = statusCode;
}

void HttpResponseHeader::setStatusMessage(const std::string &statusMessage) {
    m_statusMessage = statusMessage;
}

void HttpResponseHeader::setHeader(const std::string &key, const std::string &value) {
    m_headers[key] = value;
}

void HttpResponseHeader::setBody(const std::string &body) {
    m_body = body;
    
    if (m_headers.find("Content-Length") == m_headers.end())
        setHeader("Content-Length", std::to_string(body.size()));
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
    if (m_body.size() > 0 && m_body.size() < KILOBYTE)
        ss << m_body;
    return ss.str();
}