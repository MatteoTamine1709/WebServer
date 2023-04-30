#include "../Request.h"
#include "../TcpServer.h"

Request::Request(const HttpRequestHeader &header, const TcpServer &server)
    : app(server.getInstance()) {
    for (const auto &header : header.getHeaders()) {
        if (header.first == "Cookie" || header.first == "cookie") {
            std::vector<std::string> l_cookies = utils::split(header.second, {";"});
            for (const auto &cookie : l_cookies) {
                std::vector<std::string> cookieParts = utils::split(cookie, {"="});
                if (cookieParts.size() == 2)
                    cookies[cookieParts[0]] = cookieParts[1];
            }
        }
        m_headers[header.first] = header.second;
    }
    baseUrl = header.getRoute();
    // fresh = true;
    host = m_headers["Host"];
    hostname = m_headers["Host"];
    if (hostname.find(":") != std::string::npos)
        hostname = hostname.substr(0, hostname.find(":"));
    ip = app.getIp();
    ips = {ip};
    method = header.getMethod();
    originalUrl = header.getUrl();
    for (const auto &param : header.getParameters())
        params[param.first] = param.second;

    path = header.getRoute();
    protocol = header.getProtocol();
    // query = header.getQuery();
    // route = header.getRoute();
    secure = utils::toLower(protocol) == "https";
    // signedCookies = {};
    stale = !fresh;
    for (const auto &domain : utils::split(host, {"."}))
        subdomains.push_back(domain);
    xhr = m_headers.find("X-Requested-With") != m_headers.end() && m_headers["X-Requested-With"] == "XMLHttpRequest";
}

std::optional<std::string> Request::accepts(const std::vector<std::string> &types) const {
    std::string accept = m_headers.at("Accept");
    if (accept.empty())
        return std::nullopt;
    std::vector<std::string> acceptTypes = utils::split(accept, {","});
    for (const auto &acceptType : acceptTypes) {
        std::vector<std::string> acceptTypeParts = utils::split(acceptType, {";"});
        std::string acceptTypePart = acceptTypeParts[0];
        if (std::find(types.begin(), types.end(), acceptTypePart) != types.end())
            return acceptTypePart;
    }
    return std::nullopt;
}

std::optional<std::string> Request::acceptsEncodings(const std::vector<std::string> &encodings) const {
    std::string acceptEncoding = m_headers.at("Accept-Encoding");
    if (acceptEncoding.empty())
        return std::nullopt;
    std::vector<std::string> acceptEncodings = utils::split(acceptEncoding, {","});
    for (const auto &acceptEncoding : acceptEncodings) {
        std::vector<std::string> acceptEncodingParts = utils::split(acceptEncoding, {";"});
        std::string acceptEncodingPart = acceptEncodingParts[0];
        if (std::find(encodings.begin(), encodings.end(), acceptEncodingPart) != encodings.end())
            return acceptEncodingPart;
    }
    return std::nullopt;
}

std::optional<std::string> Request::acceptsLanguages(const std::vector<std::string> &languages) const {
    std::string acceptLanguage = m_headers.at("Accept-Language");
    if (acceptLanguage.empty())
        return std::nullopt;
    std::vector<std::string> acceptLanguages = utils::split(acceptLanguage, {","});
    for (const auto &acceptLanguage : acceptLanguages) {
        std::vector<std::string> acceptLanguageParts = utils::split(acceptLanguage, {";"});
        std::string acceptLanguagePart = acceptLanguageParts[0];
        if (std::find(languages.begin(), languages.end(), acceptLanguagePart) != languages.end())
            return acceptLanguagePart;
    }
    return std::nullopt;
}

std::optional<std::string> Request::get(const std::string &name) const {
    if (m_headers.find(name) != m_headers.end())
        return m_headers.at(name);
    return std::nullopt;
}

std::optional<std::string> Request::is(const std::string &type) const {
    std::string contentType = m_headers.at("Content-Type");
    if (contentType.empty())
        return std::nullopt;
    std::vector<std::string> contentTypes = utils::split(contentType, {";"});
    std::string contentTypePart = contentTypes[0];
    if (contentTypePart == type)
        return contentTypePart;
    return std::nullopt;
}

std::optional<Request::Range> Request::range(size_t size) const {
    std::string range = m_headers.at("Range");
    if (range.empty())
        return std::nullopt;
    std::vector<std::string> ranges = utils::split(range, {"="});
    std::string rangeType = ranges[0];
    std::string rangePart = ranges[1];
    std::vector<std::string> rangeParts = utils::split(rangePart, {"-"});
    std::string rangeStart = rangeParts[0];
    std::string rangeEnd = rangeParts[1];
    if (rangeStart.empty()) {
        size_t end = std::stoul(rangeEnd);
        return Request::Range{rangeType, size - end, size - 1};
    }
    if (rangeEnd.empty()) {
        size_t start = std::stoul(rangeStart);
        return Request::Range{rangeType, start, size - 1};
    }
    size_t start = std::stoul(rangeStart);
    size_t end = std::stoul(rangeEnd);
    return Request::Range{rangeType, start, end};
}

