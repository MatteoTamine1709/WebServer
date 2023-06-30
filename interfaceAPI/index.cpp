#include <fstream>
#include <iostream>
#include <sstream>

#include "src/Request.h"
#include "src/Response.h"

extern "C" {
void get(const Request &req, Response &res);
void post(const Request &req, Response &res);
}

void get(const Request &req, Response &res) {
    return res.sendFile("./app/dist/index.html");
}

void post(const Request &req, Response &res) {
    std::cout << req.body.dump(2) << std::endl;
    return res.json({{"message", "Hello World"}});
}