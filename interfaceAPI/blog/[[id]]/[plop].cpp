// never accessed, just for test
#include <fstream>
#include <sstream>

#include "src/Request.h"
#include "src/Response.h"

extern "C" {
void get(const Request &req, Response &res);
}

void get(const Request &req, Response &res) {
    return res.json({
        {"url", "blog/XXX/[plop]"},
    });
}