// /user
// /user/XXX#include "src/Request.h"
#include "src/Response.h"

#include <fstream>
#include <sstream>

extern "C" {
    void get(const Request &req, Response &res);
}

void get(const Request &req, Response &res) {
    return res.json({
        {"url", "user/XXX or user"},
    });
}