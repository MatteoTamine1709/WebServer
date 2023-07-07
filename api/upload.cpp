#include <fstream>
#include <iostream>

#include "src/Request.h"
#include "src/Response.h"

extern "C" {
void get(Request &req, Response &res);
void post(Request &req, Response &res);
}

void get(Request &req, Response &res) {
    res.sendFile("./app/dist/upload.html");
}

void post(Request &req, Response &res) {
    for (auto &[_, fileInfo] : req.files) {
        std::cout << "Filename: " << fileInfo.name << std::endl;
        std::cout << "Mimetype: " << fileInfo.mimetype << std::endl;
        std::cout << "Size: " << fileInfo.size << std::endl;
        // Save to download folder
        std::ofstream file("./download/" + fileInfo.name);
        int bytes = 0;
        do {
            char buffer[4 * ONE_MEGABYTE];
            bytes = fileInfo.read(buffer, 4 * ONE_MEGABYTE);
            file.write(buffer, bytes);
        } while (bytes > 0);
        file.close();
        std::cout << "File saved to ./download/" << fileInfo.name << std::endl;
    }
    res.sendFile("./app/dist/upload.html");
}