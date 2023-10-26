#include "src/Middleware.h"
#include "src/Request.h"
#include "src/Response.h"
#include "src/Result.h"

extern "C" const std::string name() { return "jsonParserMiddleware"; };
extern "C" const std::pair<std::vector<std::string>, std::vector<std::string>>
use() {
    return {{"/*"}, {"jsonParserMiddleware"}};
};

extern "C" void jsonParserMiddleware(Request &req, Response &res, Next_t next) {
    if (req.header("Content-Type").value_or("").find("application/json") ==
        std::string::npos)
        return next();
    try {
        const auto &result = req.readBody();
        if (!result.isOk())
            return res.status(result.err().code).send(result.err().message);

        const std::string &body = result.unwrap();
        if (body.empty()) return next();

        req.body = nlohmann::json::parse(body);
    } catch (const std::exception &e) {
        return res.status(400).json({"Error", "Invalid JSON"});
    }
    next();
}