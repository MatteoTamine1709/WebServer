#include "../../src/HttpRequestHeader.h"
#include "../../src/HttpResponseHeader.h"

#include <fstream>
#include <sstream>

extern "C" {
    HttpResponseHeader get(const HttpRequestHeader &header);
    HttpResponseHeader post(const HttpRequestHeader &header);
    HttpResponseHeader put(const HttpRequestHeader &header);
}


HttpResponseHeader get(const HttpRequestHeader &header) {
    HttpResponseHeader reponse{};

    reponse.setProtocol("HTTP/1.1");
    reponse.setStatusCode("200");
    reponse.setStatusMessage("OK");
    reponse.setHeader("Content-Type", "text/html");
    std::ifstream file("./public/index.html");
    std::stringstream ss;
    ss << file.rdbuf();
    reponse.setBody(ss.str());
    return reponse;
}

HttpResponseHeader post(const HttpRequestHeader &header) {
    HttpResponseHeader reponse{};

    reponse.setProtocol("HTTP/1.1");
    reponse.setStatusCode("200");
    reponse.setStatusMessage("OK");
    reponse.setHeader("Content-Type", "application/json");
    reponse.setBody("{\"message\": \"USER\"}");
    return reponse;
}

HttpResponseHeader put(const HttpRequestHeader &header) {
    HttpResponseHeader reponse{};

    reponse.setProtocol("HTTP/1.1");
    reponse.setStatusCode("200");
    reponse.setStatusMessage("OK");
    reponse.setHeader("Content-Type", "application/json");
    reponse.setBody("{\"message\": \"PUT MA GUEULE\"}");
    return reponse;
}