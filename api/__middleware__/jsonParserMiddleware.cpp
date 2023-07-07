#include "src/Middleware.h"
#include "src/Request.h"
#include "src/Response.h"

extern "C" const std::string name() { return "jsonParserMiddleware"; };
extern "C" const std::pair<std::vector<std::string>, std::vector<std::string>>
use() {
    return {{"/*"}, {"jsonParserMiddleware"}};
};

extern "C" void jsonParserMiddleware(Request &req, Response &res, Next_t next) {
    std::cout << "TTAYAAAA" << std::endl;
    if (req.header("Content-Type").value_or("").find("application/json") ==
        std::string::npos)
        return next();
    std::string body = req.body["blob"];
    if (body.empty()) return next();
    try {
        req.body = nlohmann::json::parse(body);
    } catch (const std::exception &e) {
        return res.status(400).json({"Error", "Invalid JSON"});
    }
    next();
}