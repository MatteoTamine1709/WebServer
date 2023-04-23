#include "src/HttpRequestHeader.h"
#include "src/HttpResponseHeader.h"
#include "src/Constants.h"
#include "src/utils.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <array>

extern "C" {
    HttpResponseHeader get(const HttpRequestHeader &header);
}


HttpResponseHeader get(const HttpRequestHeader &header) {
    
    HttpResponseHeader response = {};
    const auto range = header.getHeader("Range");
    std::cout << "range: " << range.value_or("HEYY") << std::endl;
    if (!range.has_value()) {
        response.setProtocol("HTTP/1.1");
        response.setStatusCode(400);
        response.setStatusMessage("Bad Request");
        response.setHeader("Content-Type", "text/html");
        response.setBody("<h1>400 Bad Request</h1>");
        return response;
    }
    const auto rangeStr = range.value();

    response.setProtocol("HTTP/1.1");
    response.setStatusCode(206);
    response.setStatusMessage("Partial Content");
    std::ifstream file("./public/assets/videos/Studio_Project.mp4", std::ios::binary);
    const auto begin = file.tellg();
    file.seekg(0, std::ios::end);
    const auto end = file.tellg();
    const auto fsize = (end - begin);

    const auto CHUNK_SIZE = MEGABYTE;
    const auto CHUNKS = fsize / CHUNK_SIZE;

    const auto rangeParts = utils::split(rangeStr, {"="});
    const auto rangeValue = rangeParts[1];
    const auto rangeValueParts = utils::split(rangeValue, {"-"});
    const auto rangeStart = std::stoi(rangeValueParts[0]);
    long rangeEnd = fsize - 1;
    if (rangeValueParts.size() > 1)
        rangeEnd = std::stoi(rangeValueParts[1]);
    rangeEnd = std::min((long) (rangeStart + CHUNK_SIZE), fsize - 1);
    const auto rangeLength = rangeEnd - rangeStart + 1;
    
    response.setHeader("Content-Type", "video/mp4");
    response.setHeader("Content-Range", "bytes " + std::to_string(rangeStart) + "-" + std::to_string(rangeEnd) + "/" + std::to_string(fsize));
    response.setHeader("Accept-Ranges", "bytes");
    response.setHeader("Connection", "keep-alive");
    response.setHeader("Cache-Control", "no-cache");

    file.seekg(rangeStart, std::ios::beg);
    std::array<char, MEGABYTE> buffer;
    file.read(buffer.data(), rangeLength);
    response.setBody(std::string(buffer.data(), rangeLength));

    return response;
}