// /entity
// /entity/1
// entity/ANYTHING
#include "src/Request.h"
#include "src/Response.h"

#include <fstream>
#include <sstream>

extern "C" {
    void get(const Request &req, Response &res);
}

void get(const Request &req, Response &res) {
    return res.json({
        {"url", "entity/[[...404]]"},
    });
}