#include "src/Middleware.h"
#include "src/Request.h"
#include "src/Response.h"

extern "C" {
void logMiddleware(const Request &req, Response &res, Next_t next);
const std::string name() { return "logMiddleware"; };
const std::pair<std::vector<std::string>, std::vector<std::string>> use() {
    return {{"/*"}, {"logMiddleware"}};
};
}

void logMiddleware(const Request &req, Response &res, Next_t next) {
    std::cout << "Log middleware: " << req.body.dump(2) << std::endl;
    next();
}