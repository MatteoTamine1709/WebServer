// Implement a Express like Request object
#ifndef HTTP_REQUEST_HEADER_INTERFACE_H
#define HTTP_REQUEST_HEADER_INTERFACE_H

#include <spdlog/spdlog.h>
#include <stdio.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

// #include "TcpServer.h"
#include "HttpRequestHeader.h"
#include "StreamFile.h"
#include "utils.h"
class TcpServer;
class Request {
   private:
    typedef struct Range_s {
        std::string type;
        std::vector<std::pair<size_t, size_t>> ranges;

        Range_s(const std::string &type,
                const std::vector<std::pair<size_t, size_t>> &ranges)
            : type(type), ranges(ranges) {}
        Range_s(const std::string &type, size_t start, size_t end)
            : type(type) {
            ranges.emplace_back(start, end);
        }

        std::string toString() {
            std::stringstream ss;
            ss << type << "=";
            for (auto &range : ranges) {
                ss << range.first << "-" << range.second << ",";
            }
            std::string s = ss.str();
            s.pop_back();
            return s;
        }

        static std::string toString(
            const std::string &type,
            const std::vector<std::pair<size_t, size_t>> &ranges) {
            std::stringstream ss;
            ss << type << "=";
            for (auto &range : ranges) {
                ss << range.first << "-" << range.second << ",";
            }
            std::string s = ss.str();
            s.pop_back();
            return s;
        }
    } Range;

   public:
    Request(const HttpRequestHeader &header, const TcpServer &server);
    ~Request() = default;

    TcpServer &app;
    std::string baseUrl;
    nlohmann::json body;
    std::unordered_map<std::string, std::string> cookies;
    bool fresh;
    std::string host;
    std::string hostname;
    std::string ip;
    std::vector<std::string> ips;
    std::string method;
    std::string originalUrl;
    std::unordered_map<std::string, std::string> params;
    std::string path;
    std::string protocol;
    std::unordered_map<std::string, std::string> query;
    std::string route;
    bool secure;
    std::vector<std::string> signedCookies;
    bool stale;
    std::vector<std::string> subdomains;
    bool xhr;
    std::unordered_map<std::string, StreamFile> files;
    StreamFile tmpFile;

    std::optional<std::string> accepts(
        const std::vector<std::string> &types) const;
    std::optional<std::string> acceptsEncodings(
        const std::vector<std::string> &encodings) const;
    std::optional<std::string> acceptsLanguages(
        const std::vector<std::string> &languages) const;
    std::optional<std::string> get(const std::string &field) const;
    std::optional<std::string> header(const std::string &field) const {
        return get(field);
    }
    std::optional<std::string> is(const std::string &type) const;

    std::optional<Range> range(size_t size) const;

   private:
    struct custom_header_hash {
        inline std::size_t operator()(const std::string &key) const {
            return std::hash<std::string>{}(utils::toLower(key));
        }
    };
    std::unordered_map<std::string, std::string, custom_header_hash> m_headers =
        {};
    int m_socket = -1;
};

#endif