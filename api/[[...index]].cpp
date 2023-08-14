#include <fstream>
#include <iostream>
#include <sstream>

#include "src/Request.h"
#include "src/Response.h"

extern "C" {
void get(Request &req, Response &res);
}

void get(Request &req, Response &res) {
    res.sendFile("./app/dist/index.html");
}