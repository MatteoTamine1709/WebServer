#include "HttpRequestHeader.h"
#include "utils.h"

#include <sstream>
#include <algorithm>
#include <filesystem>

#include <iostream>

HttpRequestHeader::HttpRequestHeader(const std::string& header) {
    std::istringstream issHeader(header);
    std::string line;
    std::getline(issHeader, line);
    std::istringstream issRequestLine(line);
    std::getline(issRequestLine, m_method, ' ');
    transform(m_method.begin(), m_method.end(), m_method.begin(), ::tolower);
    std::string route;
    std::getline(issRequestLine, route, ' ');
    std::getline(issRequestLine, m_protocol, '\r');
    while (std::getline(issHeader, line)) {
        if (line.empty())
            break;
        std::istringstream issHeaderInfo(line);
        std::string key;
        std::getline(issHeaderInfo, key, ':');
        std::string value;
        std::getline(issHeaderInfo, value);
        value.erase(std::remove(value.begin(), value.end(), '\n'), value.end());
        value.erase(std::remove(value.begin(), value.end(), '\r'), value.end());
        m_headers[key] = value;
    }
    if (m_headers.find("Content-Length") != m_headers.end()) {
        int contentLength = std::stoi(m_headers["Content-Length"]);
        m_body.resize(contentLength);
        issHeader.read(m_body.data(), contentLength);
    }
    std::vector<std::string> urlParts = utils::split(route, {"?"});
    m_url = fs::weakly_canonical(urlParts[0]);
    if (m_url.string().back() == '/')
        m_url = m_url.string().substr(0, m_url.string().length() - 1);
    m_path = m_url;

    if (urlParts.size() > 1) {
        std::vector<std::string> parameters = utils::split(urlParts[1], {"&"});
        for (const std::string& parameter : parameters) {
            std::vector<std::string> parameterParts = utils::split(parameter, {"="});
            if (parameterParts.size() > 1)
                m_parameters[parameterParts[0]] = parameterParts[1];
        }
    }
    
}

std::string HttpRequestHeader::getMethod() const {
    return m_method;
}

fs::path HttpRequestHeader::getPath() const {
    return m_path;
}

fs::path HttpRequestHeader::getRoute() const {
    return m_route;
}

std::string HttpRequestHeader::getProtocol() const {
    return m_protocol;
}

std::optional<std::string> HttpRequestHeader::getHeader(const std::string& key) const {
    if (m_headers.find(key) == m_headers.end())
        return std::nullopt;
    return m_headers.at(key);
}

std::unordered_map<std::string, std::string> HttpRequestHeader::getHeaders() const {
    return m_headers;
}

fs::path HttpRequestHeader::getUrl() const {
    return m_url;
}

std::string HttpRequestHeader::getRemoteAddress() const {
    if (m_headers.find("Remote-Address") != m_headers.end())
        return m_headers.at("Remote-Address");
    if (m_headers.find("X-Forwarded-For") != m_headers.end())
        return m_headers.at("X-Forwarded-For");
    return "";
}

std::string HttpRequestHeader::getRemoteUser() const {
    if (m_headers.find("Remote-User") != m_headers.end())
        return m_headers.at("Remote-User");
    return "";
}

std::string HttpRequestHeader::getReferrer() const {
    if (m_headers.find("Referer") != m_headers.end())
        return m_headers.at("Referer");
    return "";
}

std::string HttpRequestHeader::getUserAgent() const {
    if (m_headers.find("User-Agent") != m_headers.end())
        return m_headers.at("User-Agent");
    return "";
}

std::string HttpRequestHeader::getContentType() const {
    if (m_headers.find("Content-Type") != m_headers.end())
        return m_headers.at("Content-Type");
    return "";
}

std::string HttpRequestHeader::getBody() const {
    return m_body;
}

std::optional<std::string> HttpRequestHeader::getParameter(const std::string& key) const {
    if (m_parameters.find(key) == m_parameters.end())
        return std::nullopt;
    return m_parameters.at(key);
}

std::unordered_map<std::string, std::string> HttpRequestHeader::getParameters() const {
    return m_parameters;
}

bool HttpRequestHeader::isEndpoint() const {
    return utils::getExtension(m_path).empty();
}

void HttpRequestHeader::setPath(const std::string& path) {
    m_path = fs::weakly_canonical(path);
}

bool HttpRequestHeader::isPathValid() const {
    return fs::exists(m_path);
}

bool HttpRequestHeader::isPathDirectory() const {
    return fs::is_directory(m_path);
}

void HttpRequestHeader::setProtocol(const std::string& protocol) {
    m_protocol = protocol;
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