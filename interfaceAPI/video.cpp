#include "src/Request.h"
#include "src/Response.h"
#include "src/Constants.h"
#include "src/utils.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <array>

extern "C" {
    void get(const Request &req, Response &res);
}


void get(const Request &req, Response &res) {
    const auto range = req.header("Range");
    if (!range.has_value())
        return res.status(400).set("Content-Type", "text/html").send("<h1>400 Bad Request</h1>");
    const auto rangeStr = range.value().data();

    std::ifstream file("./public/assets/videos/Studio_Project.mp4", std::ios::binary);
    const auto begin = file.tellg();
    file.seekg(0, std::ios::end);
    const auto end = file.tellg();
    const auto fsize = (end - begin);

    const auto CHUNK_SIZE = ONE_MEGABYTE;
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

    file.seekg(rangeStart, std::ios::beg);
    std::array<char, ONE_MEGABYTE> buffer;
    file.read(buffer.data(), rangeLength);

    return res.status(206).set({
        {"Content-Type", "video/mp4"},
        {"Content-Range", "bytes " + std::to_string(rangeStart) + "-" + std::to_string(rangeEnd) + "/" + std::to_string(fsize)},
        {"Accept-Ranges", "bytes"},
        {"Connection", "keep-alive"},
        {"Cache-Control", "no-cache"}
    }).send(std::string(buffer.data(), rangeLength));
}