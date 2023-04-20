#include "src/HttpRequestHeader.h"
#include "src/HttpResponseHeader.h"

#include <fstream>
#include <sstream>

extern "C" {
    HttpResponseHeader get(const HttpRequestHeader &header);
}


HttpResponseHeader get(const HttpRequestHeader &header) {
    HttpResponseHeader reponse{};

    reponse.setProtocol("HTTP/1.1");
    reponse.setStatusCode("200");
    reponse.setStatusMessage("OK");
    reponse.setHeader("Content-Type", "text/html");
    std::ifstream file("./public/404.html");
    std::stringstream ss;
    ss << file.rdbuf();
    reponse.setBody(ss.str());
    return reponse;
}