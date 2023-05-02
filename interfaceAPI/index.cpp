#include "src/Request.h"
#include "src/Response.h"

#include <fstream>
#include <sstream>
#include <iostream>

extern "C" {
    void get(const Request &req, Response &res);
    void post(const Request &req, Response &res);
}

void get(const Request &req, Response &res) {
    return res.sendFile("./public/index.html");
}

void post(const Request &req, Response &res) {
    std::cout << req.body.dump(2) << std::endl;
    return res.json({
        {"message", "Hello World"}
    });
}