#include "HttpRequestHeader.h"
#include "utils.h"

#include <sstream>
#include <algorithm>
#include <iostream>
#include <filesystem>

HttpRequestHeader::HttpRequestHeader(const std::string& header) {
    std::istringstream issHeader(header);
    std::string line;
    std::getline(issHeader, line);
    std::istringstream issRequestLine(line);
    std::getline(issRequestLine, m_method, ' ');
    transform(m_method.begin(), m_method.end(), m_method.begin(), ::tolower);
    std::getline(issRequestLine, m_path, ' ');
    m_path = std::filesystem::path(m_path).string();
    std::getline(issRequestLine, m_protocol, ' ');
    while (std::getline(issHeader, line)) {
        if (line.empty())
            break;
        std::istringstream issHeaderInfo(line);
        std::string key;
        std::getline(issHeaderInfo, key, ':');
        std::string value;
        std::getline(issHeaderInfo, value);
        value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
        value.erase(std::remove(value.begin(), value.end(), '\n'), value.end());
        value.erase(std::remove(value.begin(), value.end(), '\r'), value.end());
        m_headers[key] = value;
    }
    if (m_headers.find("Content-Length") != m_headers.end()) {
        int contentLength = std::stoi(m_headers["Content-Length"]);
        m_body.resize(contentLength);
        issHeader.read(m_body.data(), contentLength);
    }
}

std::string HttpRequestHeader::getMethod() const {
    return m_method;
}

std::string HttpRequestHeader::getPath() const {
    return m_path;
}

std::string HttpRequestHeader::getAbsolutePath() const {
    return std::filesystem::current_path().string() + m_path;
}

std::string HttpRequestHeader::getCanonicalPath() const {
    return std::filesystem::canonical(getAbsolutePath()).string();
}

std::string HttpRequestHeader::getProtocol() const {
    return m_protocol;
}

std::optional<std::string> HttpRequestHeader::getHeader(const std::string& key) const {
    if (m_headers.find(key) == m_headers.end())
        return std::nullopt;
    return m_headers.at(key);
}

std::string HttpRequestHeader::getBody() const {
    return m_body;
}

void HttpRequestHeader::setPath(const std::string& path) {
    m_path = path;
}

bool HttpRequestHeader::isPathValid() const {
    return std::filesystem::exists(getAbsolutePath());
}

void HttpRequestHeader::setHeader(const std::string& key, const std::string& value) {
    m_headers[key] = value;
}

void HttpRequestHeader::setBody(const std::string& body) {
    m_body = body;
}

// Operators
std::ostream& operator<<(std::ostream& os, const HttpRequestHeader& header) {
    os << header.m_method << " " << header.m_path << " " << header.m_protocol << std::endl;
    for (const auto& [key, value] : header.m_headers) {
        os << key << ": " << value << std::endl;
    }
    os << "\n" << std::endl;
    os << "BODY:" << std::endl;
    os << header.m_body;
    return os;
}