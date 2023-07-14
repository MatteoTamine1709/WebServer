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
    if (req.tmpFile.getSize() == 0) return next();

    try {
        std::string boundary =
            "--" + contentType.substr(contentType.find("boundary=") + 9);
        // Read whole stream of tmpFile and parse it and store it in
        std::string filename;
        req.tmpFile.resetCursor();
        size_t size = 0;
        do {
            std::string s = req.tmpFile.readLine();
            if (s.find(boundary + "--") != std::string::npos) {
                // End of multipart/form-data
                if (!filename.empty()) {
                    req.files[filename].size = size;
                    req.files[filename].resetCursor();
                    size = 0;
                    req.files[filename].close();
                    filename = "";
                }
                break;
            } else if (s.find(boundary) != std::string::npos) {
                // Read headers
                if (!filename.empty()) {
                    req.files[filename].size = size;
                    req.files[filename].resetCursor();
                    size = 0;
                    req.files[filename].close();
                    filename = "";
                }
                std::string contentDisposition = req.tmpFile.readLine();
                if (contentDisposition.find("filename=\"") ==
                    std::string::npos) {
                    // This is a variable parsing
                    std::string name = contentDisposition.substr(
                        contentDisposition.find("name=\"") + 6,
                        contentDisposition.find(
                            "\"", contentDisposition.find("name=\"") + 6) -
                            contentDisposition.find("name=\"") - 6);
                    req.tmpFile.readLine();  // Empty line after headers
                    std::string value = req.tmpFile.readLine();
                    value = value.substr(0, value.size() - 2);
                    req.body[name] = value;
                    continue;
                }
                filename = contentDisposition.substr(
                    contentDisposition.find("filename=\"") + 10,
                    contentDisposition.find(
                        "\"", contentDisposition.find("filename=\"") + 10) -
                        contentDisposition.find("filename=\"") - 10);
                std::string contentType(req.tmpFile.readLine());
                std::string mimetype = contentType.substr(
                    contentType.find("Content-Type: ") + 14,
                    contentType.find("\n",
                                     contentType.find("Content-Type: ") + 14) -
                        contentType.find("Content-Type: ") - 14);
                // Make the file in tmp as we don't know if the end user want to
                // save it
                req.files[filename] = StreamFile("/tmp/" + filename);
                req.files[filename].mimetype = mimetype;
                req.files[filename].name = filename;
                req.files[filename].size = 0;

                req.tmpFile.readLine();
            } else {
                // Append to file
                size_t bytes = req.files[filename].write(s);
                size += bytes;
            }
        } while (!req.tmpFile.isEnd());
    } catch (const std::exception &e) {
        return res.status(400).json({{"error", e.what()}});
    }
    next();
}