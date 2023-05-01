#include "src/Request.h"
#include "src/Response.h"
#include "src/Middleware.h"

extern "C" {
    void logMiddleware(const Request &req, Response &res, Next_t next);
    const std::string name() { return "logMiddleware"; };
    const std::pair<std::vector<std::string>, std::vector<std::string>> use() {
        return {
            {"/entity/*/image"},
            {"logMiddleware"}
        };
    };
}

void logMiddleware(const Request &req, Response &res, Next_t next) {
    std::cout << "LOGGGG => " << req.method << " " << req.baseUrl << std::endl;
    next();
}