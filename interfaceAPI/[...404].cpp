#include "src/Request.h"
#include "src/Response.h"

#include <fstream>
#include <sstream>

extern "C" {
    void get(const Request &req, Response &res);
}

void get(const Request &req, Response &res) {
    return res.status(404).sendFile("./app/dist/404.html");
}