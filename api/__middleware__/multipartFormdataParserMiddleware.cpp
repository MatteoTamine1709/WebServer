#include "src/Middleware.h"
#include "src/Request.h"
#include "src/Response.h"

extern "C" const std::string name() {
    return "multipartFormDataParserMiddleware";
};
extern "C" const std::pair<std::vector<std::string>, std::vector<std::string>>
use() {
    return {{"/*"}, {"multipartFormDataParserMiddleware"}};
}

extern "C" void multipartFormDataParserMiddleware(Request &req, Response &res,
                                                  Next_t next) {
    std::cout << "AAA" << std::endl;
    std::string contentType = req.header("Content-Type").value_or("");
    if (contentType.find("multipart/form-data") == std::string::npos)
        return next();

    // try {
    // std::string boundary =
    //     "--" + contentType.substr(contentType.find("boundary=") + 9);
    // Read whole stream of tmpFile and parse it and store it in
    // std::cout << "BOUNDARY!!: " << boundary << std::endl;
    // int bytes = 0;
    // std::string filename;
    // do {
    // std::cout << "LOOP!" << std::endl;
    // char buffer[1024];
    // bytes = req.tmpFile.read(buffer, 1024);
    // std::string s(buffer, bytes);
    // std::cout << "=> " << s << std::endl;
    // if (s.find(boundary + "--") != std::string::npos) {
    //     // Append to file
    //     std::cout << "END OF MULTIPART" << std::endl;
    //     break;
    // } else if (s.find(boundary) != std::string::npos) {
    //     // TODO: New start of file, parse header info
    //     std::cout << "NEW FILE" << std::endl;
    //     std::string headers = s.substr(0, s.find("\n\r\n"));
    //     filename = headers.substr(
    //         headers.find("filename=\"") + 10,
    //         headers.find("\"", headers.find("filename=\"") + 10) -
    //             headers.find("filename=\"") - 10);
    //     std::cout << filename << std::endl;
    //     std::string mimetype = headers.substr(
    //         headers.find("Content-Type: ") + 14,
    //         headers.find("\n", headers.find("Content-Type: ") + 14) -
    //             headers.find("Content-Type: ") - 14);
    //     std::cout << mimetype << std::endl;
    //     req.files[filename] = StreamFile(filename);
    //     req.files[filename].mimetype = mimetype;
    //     req.files[filename].name = filename;
    //     req.files[filename].size = 0;

    //     // Append to file
    //     std::string body = s.substr(s.find("\n\r\n") + 3);
    //     std::cout << body << std::endl;
    //     req.files[filename].write(body);
    // } else {
    //     // Append to file
    // req.files[filename].write(s);
    // }
    // } while (bytes > 0 && !req.tmpFile.isEnd());

    // find boundary
    // std::vector<std::string> parts;
    // // Find start
    // // std::cout << "UPLOAD" << std::endl;
    // while (blob.size() > boundary.size() + 2) {
    //     size_t startOfPart =
    //         blob.find(boundary) + boundary.size() + 2;  // +2 for the
    //         \n\r
    //     size_t endOfPart = blob.find(boundary, startOfPart);
    //     if (endOfPart == std::string::npos) break;
    //     std::cout << startOfPart << ", " << endOfPart << std::endl;
    //     parts.push_back(blob.substr(startOfPart, endOfPart -
    //     startOfPart)); blob.erase(0, endOfPart);
    // }

    // for (const auto &part : parts) {
    //     // read headers
    //     std::string headers = part.substr(0, part.find("\n\r\n"));
    //     std::string body = part.substr(part.find("\n\r\n") + 3);
    //     std::string filename = headers.substr(
    //         headers.find("filename=\"") + 10,
    //         headers.find("\"", headers.find("filename=\"") + 10) -
    //             headers.find("filename=\"") - 10);
    //     // save file
    //     std::ofstream file("./download/" + filename);
    //     file << body;
    //     file.close();
    // }
    // std::cout << "====================" << req.tmpFile << std::endl;
    // } catch (const std::exception &e) {
    //     return res.status(400).json({"Error", "Invalid JSON"});
    // }
    next();
}