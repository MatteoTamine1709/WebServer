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
    reponse.setStatusCode(302);
    reponse.setStatusMessage("Found");
    reponse.setHeader("Location", "http://localhost:8080/user/1");
    return reponse;
}