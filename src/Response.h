// Implement a Express like Reqponse object
#ifndef HTTP_RESPONSE_HEADER_INTERFACE_H
#define HTTP_RESPONSE_HEADER_INTERFACE_H

#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Request.h"
#include "utils.h"

class TcpServer;

class Response {
   public:
    Response() = delete;
    Response(const Request &header);
    ~Response() = default;

    TcpServer *app;
    bool headersSent = false;
    Request req;

    Response &append(const std::string &field, const std::string &value);
    Response &attachment(const std::string &filename);
    Response &cookie(
        const std::string &name, const std::string &value,
        const std::unordered_map<std::string, std::string> &options = {});
    Response &cookie(
        const std::string &name, const nlohmann::json &value,
        const std::unordered_map<std::string, std::string> &options = {});
    Response &clearCookie(
        const std::string &name,
        const std::unordered_map<std::string, std::string> &options);
    void download(const std::string &path, const std::string &filename,
                  std::unordered_map<std::string, std::string> options = {});
    void end();
    void format(const std::unordered_map<std::string, void (*)()> &obj);
    std::optional<std::string> get(const std::string &field);
    void json(const nlohmann::json &body);
    // void jsonp(const nlohmann::json &body);
    void links(const std::unordered_map<std::string, std::string> &links);
    void location(const std::string &path);
    void redirect(const std::string &path);
    void redirect(int statusCode, const std::string &path);
    // void render(const std::string &view, const nlohmann::json &options);
    void send(const char *data);
    void send(const std::string &data);
    void send(const nlohmann::json &data);
    void sendFile(
        const std::string &path,
        const std::unordered_map<std::string, std::string> &options = {});
    void sendStatus(int statusCode);
    Response &set(const std::string &field, const std::string &value);
    Response &set(const std::unordered_map<std::string, std::string> &obj);
    Response &status(int code);
    Response &type(const std::string &type);
    Response &vary(const std::string &field);

    std::string toString();
    std::string toReadableString();

    int getStatusCode() const { return m_statusCode; }
    std::string getStatusMessage() const {
        return m_statusMessages.at(m_statusCode);
    }

    std::unordered_map<std::string, std::string> getHeaders() const {
        std::unordered_map<std::string, std::string> headers = m_headers;
        // build cookies
        std::string cookies;
        for (auto &cookie : m_cookies) {
            cookies += cookie.first + "=" + cookie.second + "; ";
        }
        if (!cookies.empty()) {
            cookies.pop_back();
            cookies.pop_back();
        }
        headers["Set-Cookie"] = cookies;
        return headers;
    }

   private:
    std::string m_data;
    std::unordered_map<int, std::string> m_statusMessages = {
        {100, "Continue"},
        {101, "Switching Protocols"},
        {102, "Processing"},
        {103, "Early Hints"},
        {200, "OK"},
        {201, "Created"},
        {202, "Accepted"},
        {203, "Non-Authoritative Information"},
        {204, "No Content"},
        {205, "Reset Content"},
        {206, "Partial Content"},
        {207, "Multi-Status"},
        {208, "Already Reported"},
        {226, "IM Used"},
        {300, "Multiple Choices"},
        {301, "Moved Permanently"},
        {302, "Found"},
        {303, "See Other"},
        {304, "Not Modified"},
        {305, "Use Proxy"},
        {306, "Switch Proxy"},
        {307, "Temporary Redirect"},
        {308, "Permanent Redirect"},
        {400, "Bad Request"},
        {401, "Unauthorized"},
        {402, "Payment Required"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {406, "Not Acceptable"},
        {407, "Proxy Authentication Required"},
        {408, "Request Timeout"},
        {409, "Conflict"},
        {410, "Gone"},
        {411, "Length Required"},
        {412, "Precondition Failed"},
        {413, "Payload Too Large"},
        {414, "URI Too Long"},
        {415, "Unsupported Media Type"},
        {416, "Range Not Satisfiable"},
        {417, "Expectation Failed"},
        {418, "I'm a teapot"},
        {421, "Misdirected Request"},
        {422, "Unprocessable Entity"},
        {423, "Locked"},
        {424, "Failed Dependency"},
        {425, "Too Early"},
        {426, "Upgrade Required"},
        {428, "Precondition Required"},
        {429, "Too Many Requests"},
        {431, "Request Header Fields Too Large"},
        {451, "Unavailable For Legal Reasons"},
        {500, "Internal Server Error"},
        {501, "Not Implemented"},
        {502, "Bad Gateway"},
        {503, "Service Unavailable"},
        {504, "Gateway Timeout"},
        {505, "HTTP Version Not Supported"},
        {506, "Variant Also Negotiates"},
        {507, "Insufficient Storage"},
        {508, "Loop Detected"},
        {510, "Not Extended"},
        {511, "Network Authentication Required"}};
    int m_statusCode = 200;
    std::unordered_map<std::string, std::string> m_headers;
    std::unordered_map<std::string, std::string> m_cookies;
};

#endif