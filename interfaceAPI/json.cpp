#include "src/Request.h"
#include "src/Response.h"
#include "src/utils.h"

extern "C" {
    void get(const Request &req, Response &res);
}


void get(const Request &req, Response &res) {
    // res.setBody("Hello World!");

    std::cout << "Baseurl: " << req.baseUrl << std::endl;
    std::cout << "Cookies: " << std::endl;
    for (auto &cookie : req.cookies) {
        std::cout << "=> " << cookie.first << ": " << cookie.second << std::endl;
    }
    std::cout << "Body: " << std::endl;
    for (auto &body : req.body) {
        std::cout << "=> " << body.first << ": " << body.second << std::endl;
    }
    std::cout << "Fresh: " << req.fresh << std::endl;
    std::cout << "Host: " << req.host << std::endl;
    std::cout << "Hostname: " << req.hostname << std::endl;
    std::cout << "Ip: " << req.ip << std::endl;
    std::cout << "Method: " << req.method << std::endl;
    std::cout << "OriginalUrl: " << req.originalUrl << std::endl;
    std::cout << "Params: " << std::endl;
    for (auto &param : req.params) {
        std::cout << "=> " << param.first << ": " << param.second << std::endl;
    }
    std::cout << "Path: " << req.path << std::endl;
    std::cout << "Protocol: " << req.protocol << std::endl;
    std::cout << "Route: " << req.route << std::endl;
    std::cout << "Secure: " << req.secure << std::endl;
    std::cout << "Stale: " << req.stale << std::endl;
    std::cout << "Subdomains: " << std::endl;
    for (auto &subdomain : req.subdomains) {
        std::cout << "=> " << subdomain << std::endl;
    }
    std::cout << "Xhr: " << req.xhr << std::endl;
    return res.status(200).json({
        {"hello", "world"},
        {"foo", "bar"}
    });
    // return res;
}