#include "src/HttpRequestHeader.h"
#include "src/HttpResponseHeader.h"
#include "src/utils.h"

#include <fstream>
#include <sstream>
#include <sys/stat.h>

extern "C" {
    HttpResponseHeader get(const HttpRequestHeader &header);
    HttpResponseHeader post(const HttpRequestHeader &header);
}


HttpResponseHeader get(const HttpRequestHeader &header) {
    HttpResponseHeader response{};
    const std::string &etag = utils::makeEtag("./public/index.html");
    if (header.getParameter("Etag").has_value() && header.getParameter("Etag").value() == etag) {
        response.setProtocol("HTTP/1.1");
        response.setStatusCode(304);
        response.setStatusMessage("Not Modified");
        response.setHeader("Content-Type", "text/html");
        response.setHeader("Etag", etag);
        return response;
    }
    response.setProtocol("HTTP/1.1");
    response.setStatusCode(200);
    response.setStatusMessage("OK");
    response.setHeader("Content-Type", "text/html");
    response.setHeader("Etag", etag);
    std::ifstream file("./app/dist/index.html");
    std::stringstream ss;
    ss << file.rdbuf();
    response.setBody(ss.str());
    return response;
}

HttpResponseHeader post(const HttpRequestHeader &header) {
    HttpResponseHeader response{};

    response.setProtocol("HTTP/1.1");
    response.setStatusCode(200);
    response.setStatusMessage("OK");
    response.setHeader("Content-Type", "application/json");
    response.setBody("{\"message\": \"Hello World!\"}");
    return response;
}