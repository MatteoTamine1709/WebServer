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
    std::string contentType = req.header("Content-Type").value_or("");
    if (contentType.find("multipart/form-data") == std::string::npos)
        return next();
    if (contentType.find("boundary=") == std::string::npos) return next();
    if (!req["Content-Length"] ||
        std::stoul(req["Content-Length"].value()) == 0)
        return next();

    std::cout << "multipartFormDataParserMiddleware" << std::endl;

    try {
        std::string boundary =
            "--" + contentType.substr(contentType.find("boundary=") + 9) +
            "\r\n";
        // Read whole stream of tmpFile and parse it and store it in
        std::string filename;
        size_t size = 0;
        std::string body = req.readWholeBody();
        std::cout << "body.size(): " << body.size() << std::endl;
        std::vector<std::string> fileContent = utils::split(body, {boundary});
        for (int i = 0; i < fileContent.size() - 1; ++i) {
            size_t newLine = fileContent[i].find("\r\n");
            std::string contentDisposition = fileContent[i].substr(0, newLine);
            if (contentDisposition.find("filename=\"") == std::string::npos) {
                // This is a variable parsing
                std::string name = contentDisposition.substr(
                    contentDisposition.find("name=\"") + 6,
                    contentDisposition.find(
                        "\"", contentDisposition.find("name=\"") + 6) -
                        contentDisposition.find("name=\"") - 6);
                size_t lineSeparator = fileContent[i].find("\r\n\r\n");
                std::string value = fileContent[i].substr(lineSeparator + 4);
                value = value.substr(0, value.size() - 2);
                req.body[name] = value;
                continue;
            }
            filename = contentDisposition.substr(
                contentDisposition.find("filename=\"") + 10,
                contentDisposition.find(
                    "\"", contentDisposition.find("filename=\"") + 10) -
                    contentDisposition.find("filename=\"") - 10);
            size_t contentTypeStart = fileContent[i].find("Content-Type: ");
            std::cout << "Content-Type: " << contentTypeStart << std::endl;
            std::string mimetype = fileContent[i].substr(
                contentTypeStart + 14,
                fileContent[i].find("\n", contentTypeStart + 14) -
                    fileContent[i].find("Content-Type: ") - 14);
            // // Make the file in tmp as we don't know if the end user want to
            // // save it
            std::cout << "File: " << filename << std::endl;
            std::cout << "Mimetype: " << mimetype << std::endl;
            req.files[filename] = StreamFile("/tmp/" + filename);
            req.files[filename].mimetype = mimetype;
            req.files[filename].name = filename;
            req.files[filename].size = 0;

            // Write the file
            req.files[filename].write(
                fileContent[i].substr(fileContent[i].find("\r\n\r\n") + 4));
            req.files[filename].resetCursor();
            req.files[filename].close();
        }

    } catch (const std::exception &e) {
        return res.status(400).json({{"error", e.what()}});
    }
    std::cout << "Files: " << req.files.size() << std::endl;
    next();
}