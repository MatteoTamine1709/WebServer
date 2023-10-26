// Implement a Express like Reqponse object
#ifndef HTTP_RESPONSE_HEADER_INTERFACE_H
#define HTTP_RESPONSE_HEADER_INTERFACE_H

#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "HttpStatus.h"
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
        return HTTP_MESSAGES.at(m_statusCode);
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

    int m_statusCode = 200;
    std::unordered_map<std::string, std::string> m_headers;
    std::unordered_map<std::string, std::string> m_cookies;
};

#endif