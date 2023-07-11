#include "HttpRequestHeader.h"

#include <curl/curl.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <sstream>

#include "utils.h"

// TODO: Refactor this constructor regarding the body
HttpRequestHeader::HttpRequestHeader(const std::string_view& header) {
    std::istringstream issHeader(header.data());
    std::string line;
    std::getline(issHeader, line);
    std::istringstream issRequestLine(line);
    std::getline(issRequestLine, m_method, ' ');
    transform(m_method.begin(), m_method.end(), m_method.begin(), ::tolower);
    std::string route;
    std::getline(issRequestLine, route, ' ');
    std::getline(issRequestLine, m_protocol, '\r');
    while (std::getline(issHeader, line)) {
        if (line.empty()) break;
        std::istringstream issHeaderInfo(line);
        std::string key;
        std::getline(issHeaderInfo, key, ':');
        std::string value;
        std::getline(issHeaderInfo, value);
        if (utils::startsWith(value, " ")) value = value.substr(1);
        value.erase(std::remove(value.begin(), value.end(), '\n'), value.end());
        value.erase(std::remove(value.begin(), value.end(), '\r'), value.end());
        m_headers[key] = value;
    }
    if (m_headers.find("Content-Length") != m_headers.end()) {
        // TODO: Make different type of handlers for different content types
        int contentLength = std::stoi(m_headers["Content-Length"]);
        size_t body_pos = header.find("\r\n\r\n");
        if (body_pos != std::string::npos) {
            m_body = header.substr(body_pos + 4, contentLength);
        }
    }
    std::vector<std::string> urlParts = utils::split(route, {"?"});
    m_url = fs::weakly_canonical(urlParts[0]);
    if (m_url.string().back() == '/')
        m_url = m_url.string().substr(0, m_url.string().size() - 1);
    m_path = m_url;
    m_route = m_url;
    if (m_route.string().empty()) m_route = "/";

    if (urlParts.size() > 1) {
        char* decodedKey =
            curl_unescape(urlParts[1].c_str(), urlParts[1].size());
        std::vector<std::string> queries =
            utils::split(std::string(decodedKey), {"&"});
        for (const std::string& query : queries) {
            std::vector<std::string> queryParts = utils::split(query, {"="});
            if (queryParts.size() > 1) {
                m_queries[queryParts[0]] = queryParts[1];
            }
        }
        curl_free(decodedKey);
    }
}

std::string_view HttpRequestHeader::getMethod() const { return m_method; }

fs::path HttpRequestHeader::getPath() const { return m_path; }

fs::path HttpRequestHeader::getRoute() const { return m_route; }

std::string_view HttpRequestHeader::getProtocol() const { return m_protocol; }

std::optional<std::string_view> HttpRequestHeader::getHeader(
    const std::string_view& key) const {
    if (m_headers.find(key.data()) == m_headers.end()) return std::nullopt;
    return m_headers.at(key.data());
}

const std::unordered_map<std::string, std::string>&
HttpRequestHeader::getHeaders() const {
    return m_headers;
}

fs::path HttpRequestHeader::getUrl() const { return m_url; }

std::string_view HttpRequestHeader::getRemoteAddress() const {
    if (m_headers.find("Remote-Address") != m_headers.end())
        return m_headers.at("Remote-Address");
    if (m_headers.find("X-Forwarded-For") != m_headers.end())
        return m_headers.at("X-Forwarded-For");
    return "";
}

std::string_view HttpRequestHeader::getRemoteUser() const {
    if (m_headers.find("Remote-User") != m_headers.end())
        return m_headers.at("Remote-User");
    return "";
}

std::string_view HttpRequestHeader::getReferrer() const {
    if (m_headers.find("Referer") != m_headers.end())
        return m_headers.at("Referer");
    return "";
}

std::string_view HttpRequestHeader::getUserAgent() const {
    if (m_headers.find("User-Agent") != m_headers.end())
        return m_headers.at("User-Agent");
    return "";
}

std::string_view HttpRequestHeader::getContentType() const {
    if (m_headers.find("Content-Type") != m_headers.end())
        return m_headers.at("Content-Type");
    return "";
}

std::string_view HttpRequestHeader::getBody() const { return m_body; }

std::optional<std::string_view> HttpRequestHeader::getParameter(
    const std::string_view& key) const {
    if (m_parameters.find(key.data()) == m_parameters.end())
        return std::nullopt;
    return m_parameters.at(key.data());
}

const std::unordered_map<std::string, std::string>&
HttpRequestHeader::getParameters() const {
    return m_parameters;
}

const std::unordered_map<std::string, std::string>&
HttpRequestHeader::getQueries() const {
    return m_queries;
}

bool HttpRequestHeader::isEndpoint() const {
    return utils::getExtension(m_path).empty();
}

void HttpRequestHeader::setPath(const std::string_view& path) {
    m_path = fs::weakly_canonical(path);
}

void HttpRequestHeader::setParameters(const fs::path& apiPath) {
    // Extract url parameters

    // routeParts: entity, aaa
    // pathParts: home, mat, dev, C++, MyYoutube, WebServer, interfaceAPI,
    // entity, [id], index.so
    // id: "aaa"

    // routeParts: entity, aaa, image
    // pathParts: home, mat, dev, C++, MyYoutube, WebServer, interfaceAPI,
    // entity, [id], image.so
    // id: "aaa"

    // routeParts: blog
    // pathParts: home, mat, dev, C++, MyYoutube, WebServer, interfaceAPI, blog,
    // [[id]], index.so
    // id: ""

    // routeParts: blog, ppp
    // pathParts: home, mat, dev, C++, MyYoutube, WebServer, interfaceAPI, blog,
    // [[id]], index.so
    // id: "ppp"

    // routeParts: blog, ppp, mmm
    // pathParts: home, mat, dev, C++, MyYoutube, WebServer, interfaceAPI, blog,
    // [[id]], [plop].so
    // id: "ppp" plop: "mmm"
    std::string pathStr = m_path.string().substr(apiPath.string().size());
    std::vector<std::string> routeParts = utils::split(m_route.string(), {"/"});
    std::vector<std::string> pathParts = utils::split(pathStr, {"/"});

    for (int i = 0; i < pathParts.size(); ++i)
        if (utils::startsWith(pathParts[i], "[")) {
            std::string parameterName = pathParts[i];
            while (utils::startsWith(parameterName, "["))
                parameterName = parameterName.substr(1);
            if (utils::startsWith(parameterName, "..."))
                parameterName = parameterName.substr(3);
            if (utils::endsWith(parameterName, ".so"))
                parameterName =
                    parameterName.substr(0, parameterName.size() - 3);
            while (utils::endsWith(parameterName, "]"))
                parameterName =
                    parameterName.substr(0, parameterName.size() - 1);
            if (i < routeParts.size()) {
                m_parameters[parameterName] = routeParts[i];
            } else {
                m_parameters[parameterName] = "";
            }
        }
}

bool HttpRequestHeader::isPathValid() const { return fs::exists(m_path); }

bool HttpRequestHeader::isPathDirectory() const {
    return fs::is_directory(m_path);
}

void HttpRequestHeader::setProtocol(const std::string_view& protocol) {
    m_protocol = protocol;
}

void HttpRequestHeader::setHeader(const std::string_view& key,
                                  const std::string_view& value) {
    m_headers[key.data()] = value;
}

void HttpRequestHeader::setBody(const std::string_view& body) { m_body = body; }

void HttpRequestHeader::complete(TcpConnection& connection) {
    if (getProtocol().empty()) setProtocol("HTTP/1.1");
    if (!getHeader("Content-Length")) return;
    int contentLength = std::stoi(m_headers["Content-Length"]);
    if (contentLength == 0) return;
    if (m_body.size() == contentLength) return;
    if (contentLength > 4ul * ONE_GIGABYTE) return;
    tmpFile = connection.readTmp(contentLength);
}

// Operators
std::ostream& operator<<(std::ostream& os, const HttpRequestHeader& header) {
    os << header.m_method << " " << header.m_path << " " << header.m_protocol
       << std::endl;
    for (const auto& [key, value] : header.m_headers) {
        os << key << ": " << value << std::endl;
    }
    os << "\n" << std::endl;
    os << "BODY:" << std::endl;
    os << header.m_body;
    return os;
}