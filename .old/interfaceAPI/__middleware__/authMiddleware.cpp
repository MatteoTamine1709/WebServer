#include "src/Request.h"
#include "src/Response.h"
#include "src/Middleware.h"

extern "C" {
    void authMiddleware(const Request &req, Response &res, Next_t next);
    const std::string name() { return "authMiddleware"; };
    const std::pair<std::vector<std::string>, std::vector<std::string>> use() {
        return {
            {"/download", "/user/*"},
            {"authMiddleware", "logMiddleware"}
        };
    };
}

void authMiddleware(const Request &req, Response &res, Next_t next) {
    std::cout << "authMiddleware" << std::endl;
    if (req.header("Authorization") == std::nullopt)
        return res.status(401).json({
            {"message", "Unauthorized"}
        });
    next();
}