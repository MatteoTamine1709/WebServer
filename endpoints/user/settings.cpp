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
    reponse.setHeader("Content-Type", "application/json");
    reponse.setBody("{\"id\": \"" + header.getParameter("id").value() + "\"}");
    return reponse;
}