#include "../Request.h"

#include "../TcpServer.h"

Request::Request(TcpConnection &connection, const TcpServer &server)
    : app(server.getInstance()), connection(connection) {}

Result<bool, HttpStatus> Request::readHeader() {
    Result<std::string, HttpStatus> result = connection.readHeader();
    if (!result.isOk()) return Result<bool, HttpStatus>::err(result.err());
    std::string header = result.unwrap();
    if (header.empty()) return Result<bool, HttpStatus>::err(BAD_REQUEST);

    std::vector<std::string> lines = utils::split(header, {"\r\n"});
    std::vector<std::string> firstLine = utils::split(lines[0], {" "});
    if (firstLine.size() != 3)
        return Result<bool, HttpStatus>::err(BAD_REQUEST);
    method = utils::toLower(firstLine[0]);
    originalUrl = firstLine[1];
    protocol = firstLine[2];
    std::vector<std::string> urlParts = utils::split(originalUrl, {"?"});
    path = urlParts[0];
    if (urlParts.size() == 2) {
        std::vector<std::string> queries = utils::split(urlParts[1], {"&"});
        for (const auto &query : queries) {
            std::vector<std::string> queryParts = utils::split(query, {"="});
            if (queryParts.size() == 2)
                this->query[queryParts[0]] = queryParts[1];
        }
    }
    for (size_t i = 1; i < lines.size(); i++) {
        std::vector<std::string> headerParts = utils::split(lines[i], {": "});
        if (headerParts.size() == 2) {
            m_headers[headerParts[0]] = headerParts[1];
            if (headerParts[0] == "Cookie" || headerParts[0] == "cookie") {
                std::vector<std::string> l_cookies =
                    utils::split(headerParts[1], {"; "});
                for (const auto &cookie : l_cookies) {
                    std::vector<std::string> cookieParts =
                        utils::split(cookie, {"="});
                    if (cookieParts.size() == 2)
                        cookies[cookieParts[0]] = cookieParts[1];
                }
            }
        }
    }
    host = m_headers["Host"];
    hostname = m_headers["Host"];
    if (hostname.find(":") != std::string::npos)
        hostname = hostname.substr(0, hostname.find(":"));
    ip = app.getIp();
    ips = {ip};
    secure = utils::toLower(protocol) == "https";
    for (const auto &domain : utils::split(host, {"."}))
        subdomains.push_back(domain);
    xhr = m_headers.find("X-Requested-With") != m_headers.end() &&
          m_headers["X-Requested-With"] == "XMLHttpRequest";
    return Result<bool, HttpStatus>::ok(true);
}

Result<std::string, HttpStatus> Request::readBody() {
    if (m_headers.find("Content-Length") == m_headers.end())
        return Result<std::string, HttpStatus>::err(BAD_REQUEST);
    size_t size = std::stoul(m_headers["Content-Length"]);
    return connection.readSize(size);
}

void Request::setParameters() {
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
    fs::path apiPath = app.getApiPath();
    // get the path AFTER the api folder using fileSystemPath
    std::string pathStr =
        fileSystemPath.string().substr(apiPath.string().size());
    std::vector<std::string> pathParts = utils::split(pathStr, {"/"});
    std::vector<std::string> routeParts = utils::split(path, {"/"});
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
                params[parameterName] = routeParts[i];
            } else {
                params[parameterName] = "";
            }
        }
}

std::optional<std::string> Request::accepts(
    const std::vector<std::string> &types) const {
    std::string accept = m_headers.at("Accept");
    if (accept.empty()) return std::nullopt;
    std::vector<std::string> acceptTypes = utils::split(accept, {","});
    for (const auto &acceptType : acceptTypes) {
        std::vector<std::string> acceptTypeParts =
            utils::split(acceptType, {";"});
        std::string acceptTypePart = acceptTypeParts[0];
        if (std::find(types.begin(), types.end(), acceptTypePart) !=
            types.end())
            return acceptTypePart;
    }
    return std::nullopt;
}

std::optional<std::string> Request::acceptsEncodings(
    const std::vector<std::string> &encodings) const {
    std::string acceptEncoding = m_headers.at("Accept-Encoding");
    if (acceptEncoding.empty()) return std::nullopt;
    std::vector<std::string> acceptEncodings =
        utils::split(acceptEncoding, {","});
    for (const auto &acceptEncoding : acceptEncodings) {
        std::vector<std::string> acceptEncodingParts =
            utils::split(acceptEncoding, {";"});
        std::string acceptEncodingPart = acceptEncodingParts[0];
        if (std::find(encodings.begin(), encodings.end(), acceptEncodingPart) !=
            encodings.end())
            return acceptEncodingPart;
    }
    return std::nullopt;
}

std::optional<std::string> Request::acceptsLanguages(
    const std::vector<std::string> &languages) const {
    std::string acceptLanguage = m_headers.at("Accept-Language");
    if (acceptLanguage.empty()) return std::nullopt;
    std::vector<std::string> acceptLanguages =
        utils::split(acceptLanguage, {","});
    for (const auto &acceptLanguage : acceptLanguages) {
        std::vector<std::string> acceptLanguageParts =
            utils::split(acceptLanguage, {";"});
        std::string acceptLanguagePart = acceptLanguageParts[0];
        if (std::find(languages.begin(), languages.end(), acceptLanguagePart) !=
            languages.end())
            return acceptLanguagePart;
    }
    return std::nullopt;
}

std::optional<std::string> Request::get(const std::string &name) const {
    if (m_headers.find(name) != m_headers.end()) return m_headers.at(name);
    return std::nullopt;
}

std::optional<std::string> Request::is(const std::string &type) const {
    std::string contentType = m_headers.at("Content-Type");
    if (contentType.empty()) return std::nullopt;
    std::vector<std::string> contentTypes = utils::split(contentType, {";"});
    std::string contentTypePart = contentTypes[0];
    if (contentTypePart == type) return contentTypePart;
    return std::nullopt;
}

std::optional<Request::Range> Request::range(size_t size) const {
    std::string range = m_headers.at("Range");
    if (range.empty()) return std::nullopt;
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
