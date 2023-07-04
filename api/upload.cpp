#include <fstream>
#include <iostream>

#include "src/Request.h"
#include "src/Response.h"

extern "C" {
void get(const Request &req, Response &res);
void post(const Request &req, Response &res);
}

void get(const Request &req, Response &res) {
    res.sendFile("./app/dist/upload.html");
}

void post(const Request &req, Response &res) {
    std::string blob = req.body["blob"];
    if (!req.header("Content-Type"))
        return res.status(400).json({"Error", "No Content-Type"});
    std::string contentType = req.header("Content-Type").value();
    if (contentType.find("multipart/form-data") == std::string::npos)
        return res.status(400).json(
            {"Error", "Content-Type must be multipart/form-data"});
    // find boundary
    std::string boundary =
        "--" + contentType.substr(contentType.find("boundary=") + 9);
    std::vector<std::string> parts;
    // Find start
    // std::cout << "UPLOAD" << std::endl;
    while (blob.size() > boundary.size() + 2) {
        size_t startOfPart =
            blob.find(boundary) + boundary.size() + 2;  // +2 for the \n\r
        size_t endOfPart = blob.find(boundary, startOfPart);
        if (endOfPart == std::string::npos) break;
        std::cout << startOfPart << ", " << endOfPart << std::endl;
        parts.push_back(blob.substr(startOfPart, endOfPart - startOfPart));
        blob.erase(0, endOfPart);
    }

    for (const auto &part : parts) {
        // read headers
        std::string headers = part.substr(0, part.find("\n\r\n"));
        std::string body = part.substr(part.find("\n\r\n") + 3);
        std::string filename = headers.substr(
            headers.find("filename=\"") + 10,
            headers.find("\"", headers.find("filename=\"") + 10) -
                headers.find("filename=\"") - 10);
        // save file
        std::ofstream file("./download/" + filename);
        file << body;
        file.close();
    }

    res.json({"Success", "Success"});
}