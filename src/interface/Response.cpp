#include "../Response.h"
#include "../Request.h"
#include "../TcpServer.h"
#include <fstream>

Response::Response(const Request &header, const TcpServer &server)
    : app(&server.getInstance()), headersSent(false), req(header), m_statusCode(200) {
}

Response& Response::append(const std::string &field, const std::string &value) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return *this;
    }
    m_headers[field] = value;
    return *this;
}

Response& Response::attachment(const std::string &filename) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return *this;
    }
    m_headers["Content-Disposition"] = "attachment; filename=\"" + filename + "\"";
    return *this;
}

Response& Response::cookie(const std::string &name, const std::string &value, const std::unordered_map<std::string, std::string> &options) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return *this;
    }
    std::string cookie = name + "=" + value;
    if (options.find("domain") != options.end())
        cookie += "; Domain=" + options.at("domain");
    if (options.find("expires") != options.end())
        cookie += "; Expires=" + options.at("expires");
    if (options.find("httpOnly") != options.end())
        cookie += "; HttpOnly";
    if (options.find("maxAge") != options.end())
        cookie += "; Max-Age=" + options.at("maxAge");
    if (options.find("path") != options.end())
        cookie += "; Path=" + options.at("path");
    if (options.find("prioriy") != options.end())
        cookie += "; Priority=" + options.at("priority");
    if (options.find("secure") != options.end())
        cookie += "; Secure";
    if (options.find("signed") != options.end() && options.at("signed") == "true")
        cookie += "; Signed";
    if (options.find("sameSite") != options.end())
        cookie += "; SameSite=" + options.at("sameSite");
    m_cookies[name] = cookie;
    return *this;
}

Response& Response::cookie(const std::string &name, const nlohmann::json &value, const std::unordered_map<std::string, std::string> &options) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return *this;
    }
    return cookie(name, value.dump(), options);
}

Response& Response::clearCookie(const std::string &name, const std::unordered_map<std::string, std::string> &options) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return *this;
    }
    std::string cookie = name + "=; Expires=Thu, 01 Jan 1970 00:00:00 GMT";
    if (options.find("domain") != options.end())
        cookie += "; Domain=" + options.at("domain");
    if (options.find("expires") != options.end())
        cookie += "; Expires=" + options.at("expires");
    if (options.find("httpOnly") != options.end())
        cookie += "; HttpOnly";
    if (options.find("maxAge") != options.end())
        cookie += "; Max-Age=" + options.at("maxAge");
    if (options.find("path") != options.end())
        cookie += "; Path=" + options.at("path");
    if (options.find("prioriy") != options.end())
        cookie += "; Priority=" + options.at("priority");
    if (options.find("secure") != options.end())
        cookie += "; Secure";
    if (options.find("signed") != options.end() && options.at("signed") == "true")
        cookie += "; Signed";
    if (options.find("sameSite") != options.end())
        cookie += "; SameSite=" + options.at("sameSite");
    m_cookies[name] = cookie;
    return *this;
}

void Response::download(const std::string &path, const std::string &filename, std::unordered_map<std::string, std::string> options) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return;
    }
    // if (options.find("maxAge") != options.end())
    //     header += "; Max-Age=" + options.at("maxAge");
    // else
    //     header += "; Max-Age=0";
    // if (options.find("root") != options.end())
    //     header += "; Root=" + options.at("root");
    // if (options.find("lastModified") != options.end())
    //     header += "; Last-Modified=" + options.at("lastModified");
    // if (options.find("headers") != options.end())
    //     header += "; Headers=" + options.at("headers");
    // if (options.find("dotfiles") != options.end())
    //     header += "; Dotfiles=" + options.at("dotfiles");
    // else
    //     header += "; Dotfiles=ignore";
    // if (options.find("acceptRanges") != options.end())
    //     header += "; Accept-Ranges";
    // if (options.find("cacheControl") != options.end())
    //     header += "; Cache-Control=" + options.at("cacheControl");
    // if (options.find("immutable") != options.end() && options.at("immutable") == "true")
    //     header += "; Immutable";
    m_headers["Content-Disposition"] = "attachment; filename=\"" + filename + "\"";
    sendFile(path);
}

void Response::end() {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return;
    }
    headersSent = true;
    // app.sendResponse(m_headers, m_cookies, m_data);
}

void Response::format(const std::unordered_map<std::string, void (*)()> &obj) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return;
    }
    std::string accept = "text/html";
    if (!req.get("accept").has_value())
        accept = req.get("accept").value();
    
    if (obj.find(accept) != obj.end())
        return obj.at(accept)();
    if (obj.find("default") != obj.end())
        return obj.at("default")();
    
    status(406).send(std::string("Not Acceptable"));
}

std::optional<std::string> Response::get(const std::string &field) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return std::nullopt;
    }
    if (m_headers.find(field) != m_headers.end())
        return m_headers.at(field);
    return std::nullopt;
}

void Response::json(const nlohmann::json &obj) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return;
    }
    m_headers["Content-Type"] = "application/json";
    send(obj.dump());
}

void Response::links(const std::unordered_map<std::string, std::string> &links) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return;
    }
    std::string link = "";
    for (auto &i : links)
        link += "<" + i.second + ">; rel=\"" + i.first + "\", ";
    link.pop_back();
    link.pop_back();
    m_headers["Link"] = link;
}

void Response::location(const std::string &url) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return;
    }
    m_headers["Location"] = url;
}

void Response::redirect(const std::string &url) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return;
    }
    m_headers["Location"] = url;
    status(302).send(std::string("Found"));
}

void Response::redirect(int statusCode, const std::string &url) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return;
    }
    m_headers["Location"] = url;
    status(statusCode).send(std::string("Found"));
}

void Response::send(const char *data) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return;
    }
    m_data = data;
    end();
}

void Response::send(const std::string &data) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return;
    }
    m_data = data;
    end();
}

void Response::send(const nlohmann::json &obj) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return;
    }
    m_data = obj.dump();
    end();
}

void Response::sendFile(const std::string &path, const std::unordered_map<std::string, std::string> &options) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return;
    }
    // if (options.find("maxAge") != options.end())
    //     header += "; Max-Age=" + options.at("maxAge");
    // else
    //     header += "; Max-Age=0";
    // if (options.find("root") != options.end())
    //     header += "; Root=" + options.at("root");
    // if (options.find("lastModified") != options.end())
    //     header += "; Last-Modified=" + options.at("lastModified");
    // if (options.find("headers") != options.end())
    //     header += "; Headers=" + options.at("headers");
    // if (options.find("dotfiles") != options.end())
    //     header += "; Dotfiles=" + options.at("dotfiles");
    // else
    //     header += "; Dotfiles=ignore";
    // if (options.find("acceptRanges") != options.end())
    //     header += "; Accept-Ranges";
    // if (options.find("cacheControl") != options.end())
    //     header += "; Cache-Control=" + options.at("cacheControl");
    // if (options.find("immutable") != options.end() && options.at("immutable") == "true")
    //     header += "; Immutable";
    m_headers["Content-Disposition"] = "inline";
    m_headers["Content-Type"] = utils::getMimeType(utils::getExtension(path));
    std::ifstream file(path, std::ios::binary);
    std::stringstream ss;
    ss << file.rdbuf();
    send(ss.str());
    // reponse.setBody(ss.str());
    // end();
}

void Response::sendStatus(int statusCode) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return;
    }
    if (m_statusMessages.find(statusCode) != m_statusMessages.end())
        m_data = m_statusMessages.at(statusCode);
    else
        m_data = std::to_string(statusCode);
    status(statusCode).end();
}

Response& Response::set(const std::string &field, const std::string &value) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return *this;
    }
    m_headers[field] = value;
    return *this;
}

Response& Response::set(const std::unordered_map<std::string, std::string> &obj) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return *this;
    }
    for (auto &i : obj)
        m_headers[i.first] = i.second;
    return *this;
}

Response& Response::status(int statusCode) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return *this;
    }
    m_statusCode = statusCode;
    return *this;
}

Response& Response::type(const std::string &type) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return *this;
    }
    if (utils::getMimeType(type) != "unknown") {
        m_headers["Content-Type"] = type;
        return *this;
    }
    if (!utils::getExtension(type).empty()) {
        m_headers["Content-Type"] = utils::getMimeType(utils::getExtension(type));
        return *this;
    }
    m_headers["Content-Type"] = type;
    return *this;
}

Response& Response::vary(const std::string &field) {
    if (headersSent) {
        spdlog::error("Headers already sent");
        return *this;
    }
    if (m_headers.find("Vary") != m_headers.end())
        m_headers["Vary"] += ", " + field;
    else
        m_headers["Vary"] = field;
    return *this;
}


std::string Response::toString() {
    std::string statusMessage = "";
    if (m_statusMessages.find(m_statusCode) != m_statusMessages.end())
        statusMessage = m_statusMessages.at(m_statusCode);
    else
        statusMessage = "Unknown";
    std::string str = "HTTP/1.1 " + std::to_string(m_statusCode) + " " + statusMessage + "\r\n";
    // build cookies
    if (!m_cookies.empty()) {
        std::string cookies = "";
        for (auto &i : m_cookies)
            cookies += i.first + "=" + i.second + "; ";
        cookies.pop_back();
        cookies.pop_back();
        str += "Set-Cookie: " + cookies + "\r\n";
    }
    if (m_headers.find("Content-Type") == m_headers.end())
        m_headers["Content-Type"] = "text/html";
    if (m_headers.find("Content-Length") == m_headers.end())
        m_headers["Content-Length"] = std::to_string(m_data.size());
    for (auto &i : m_headers)
        str += i.first + ": " + i.second + "\r\n";
    str += "\r\n";
    str += m_data;
    return str;
}

std::string Response::toReadableString() {
    std::string statusMessage = "";
    if (m_statusMessages.find(m_statusCode) != m_statusMessages.end())
        statusMessage = m_statusMessages.at(m_statusCode);
    else
        statusMessage = "Unknown";
    std::string str = "HTTP/1.1 " + std::to_string(m_statusCode) + " " + statusMessage + "\n";
    // build cookies
    if (!m_cookies.empty()) {
        std::string cookies = "";
        for (auto &i : m_cookies)
            cookies += i.first + "=" + i.second + "; ";
        cookies.pop_back();
        cookies.pop_back();
        str += "Set-Cookie: " + cookies + "\n";
    }
    if (m_headers.find("Content-Type") == m_headers.end())
        m_headers["Content-Type"] = "text/html";
    if (m_headers.find("Content-Length") == m_headers.end())
        m_headers["Content-Length"] = std::to_string(m_data.size());
    for (auto &i : m_headers)
        str += i.first + ": " + i.second + "\n";
    str += "\n";
    if (m_data.size() > 1000)
        str += "Body { ... }";
    else
        str += m_data;
    return str;
}