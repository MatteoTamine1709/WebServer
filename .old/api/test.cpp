#include "src/HttpRequestHeader.h"
#include "src/HttpResponseHeader.h"
#include "src/utils.h"

#include <fstream>
#include <sstream>
#include <iostream>

extern "C" {
    HttpResponseHeader get(const HttpRequestHeader &header);
}


HttpResponseHeader get(const HttpRequestHeader &header) {
    HttpResponseHeader response{};
    const std::string &etag = utils::makeEtag("./api/test.cpp");
    response.setProtocol("HTTP/1.1");
    response.setHeader("Content-Type", "application/json");
    response.setHeader("Etag", etag);
    if (header.getHeader("If-None-Match").has_value() && header.getHeader("If-None-Match").value() == etag) {
        response.setStatusCode(304);
        response.setStatusMessage("Not Modified");
        return response;
    }

    response.setStatusCode(200);
    response.setStatusMessage("OK");
    response.setBody("{\"message\": \"" + std::string(header.getParameter("plop").value_or("UNDEFINED")) + "\"}");
    return response;
}