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
    std::cout << "# files:" << req.files.size() << std::endl;
    for (auto &[_, fileInfo] : req.files) {
        std::cout << "Filename: " << fileInfo.name << std::endl;
        std::cout << "Mimetype: " << fileInfo.mimetype << std::endl;
        std::cout << "Size: " << fileInfo.size << std::endl;
        // Save to download folder
        if (fileInfo.name.find_last_of("/") != std::string::npos)
            fs::create_directories(
                "./download/" +
                fileInfo.name.substr(0, fileInfo.name.find_last_of("/")) + "/");
        std::ofstream file("./download/" + fileInfo.name);
        char *buffer = new char[fileInfo.size];
        size_t bytes = fileInfo.read(buffer, fileInfo.size);
        file.write(buffer, bytes);
        file.close();
        std::cout << "File saved to ./download/" << fileInfo.name << std::endl;
    }
    // Redirect to upload
    res.redirect("/upload");
}