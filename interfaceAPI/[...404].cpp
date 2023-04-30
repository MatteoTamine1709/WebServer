#include "src/Request.h"
#include "src/Response.h"

#include <fstream>
#include <sstream>

extern "C" {
    void get(const Request &req, Response &res);
}

void get(const Request &req, Response &res) {
    // HttpResponseHeader reponse{};
    return res.sendFile("./app/dist/404.html");
    // reponse.setProtocol("HTTP/1.1");
    // reponse.setStatusCode(200);
    // reponse.setStatusMessage("OK");
    // reponse.setHeader("Content-Type", "text/html");
    // std::ifstream file("./app/dist/404.html");
    // std::stringstream ss;
    // ss << file.rdbuf();
    // reponse.setBody(ss.str());
    // return reponse;
}