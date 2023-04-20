#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/common.h>
#include <spdlog/pattern_formatter.h>

#include "./HttpResponseHeader.h"
#include "./HttpRequestHeader.h"

#include <iostream>

#include <spdlog/fmt/fmt.h>

extern std::vector<std::string> LOGGER_FORMAT;

typedef struct LoggedInfo_t {
    std::string remoteAddress;
    std::string remoteUser;
    std::string route;
    std::string method;
    std::string url;
    std::string httpVersion;
    std::string status;
    std::string referrer;
    std::string userAgent;
    std::string statusMessage;
    std::string responseTime;

    std::unordered_map<std::string, std::string> headers;

    LoggedInfo_t() = default;

    LoggedInfo_t(const HttpRequestHeader& request, const HttpResponseHeader& response, int64_t responseTime) {
        this->remoteAddress = request.getRemoteAddress();
        this->remoteUser = request.getRemoteUser();
        this->route = request.getRoute();
        this->method = utils::toUpper(request.getMethod());
        this->url = request.getUrl();
        this->httpVersion = request.getProtocol();
        this->status = response.getStatusCode();
        this->referrer = request.getReferrer();
        this->userAgent = request.getUserAgent();
        this->statusMessage = response.getStatusMessage();
        this->responseTime = std::to_string(responseTime);
        for (const auto &[key, value] : request.getHeaders())
            this->headers[key] = value;
        for (const auto &[key, value] : response.getHeaders())
            this->headers[key] = value;
    }
} LoggedInfo;

template<>
struct fmt::formatter<LoggedInfo> {
    std::string token = "";
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(), end = ctx.end();
        while (it != end && *it != '}') token += *it++;
        return it;
    }

    template <typename FormatContext>
    auto format(const LoggedInfo& logInfo, FormatContext& ctx) -> decltype(ctx.out()) {
        if (token == "remoteAddress" || token == "remote-address" || token == "remote-addr") return format_to(ctx.out(), "{}", logInfo.remoteAddress);
        if (token == "remoteUser" || token == "remote-user") return format_to(ctx.out(), "{}", logInfo.remoteUser);
        if (token == "route") return format_to(ctx.out(), "{}", logInfo.route);
        int64_t statusValue = std::stoll(logInfo.status);
        std::string color = "\033[1;32m";
        if (statusValue >= 400 && statusValue < 500) color = "\033[1;33m";
        else if (statusValue >= 500) color = "\033[1;31m";
        if (token == "method") return format_to(ctx.out(), color + "{}\033[1;0m", logInfo.method);
        if (token == "url") return format_to(ctx.out(), "{}", logInfo.url);
        if (token == "httpVersion" || token == "http-version") return format_to(ctx.out(), "{}", logInfo.httpVersion);
        if (token == "status") return format_to(ctx.out(), color + "{}\033[1;0m", logInfo.status);
        if (token == "referrer") return format_to(ctx.out(), "{}", logInfo.referrer);
        if (token == "userAgent" || token == "user-agent") return format_to(ctx.out(), "{}", logInfo.userAgent);
        if (token == "statusMessage" || token == "status-message") return format_to(ctx.out(), "{}", logInfo.statusMessage);
        if (token == "responseTime" || token == "response-time") return format_to(ctx.out(), "{}", logInfo.responseTime);
        // token = "header[header-name]"
        if (token.rfind("header[", 0) == 0) {
            std::string header = token.substr(7);
            header.pop_back();
            if (logInfo.headers.find(header) != logInfo.headers.end())
                return format_to(ctx.out(), "{}", logInfo.headers.at(header));
            return format_to(ctx.out(), "-");
        }

        return format_to(ctx.out(), "{}", token);
    }
};

class devFomat : public spdlog::custom_flag_formatter
{
public:
    void format(const spdlog::details::log_msg &msg, const std::tm &, spdlog::memory_buf_t &dest) override {
        dest.append(msg.payload.data(), msg.payload.data() + msg.payload.size());
    }

    std::unique_ptr<custom_flag_formatter> clone() const override {
        return spdlog::details::make_unique<devFomat>();
    }
};

#endif