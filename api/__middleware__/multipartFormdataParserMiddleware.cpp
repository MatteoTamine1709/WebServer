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

    try {
        std::string boundary =
            "--" + contentType.substr(contentType.find("boundary=") + 9);
        // Read whole stream of tmpFile and parse it and store it in
        std::string filename;
        req.tmpFile.resetCursor();
        do {
            std::string s = req.tmpFile.readLine();
            if (s.find(boundary + "--") != std::string::npos) {
                break;
            } else if (s.find(boundary) != std::string::npos) {
                // Read headers
                std::string contentDisposition = req.tmpFile.readLine();
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
                req.files[filename].write(s);
            }
        } while (!req.tmpFile.isEnd());

        for (auto &[_, fileInfo] : req.files) {
            fileInfo.size = fileInfo.getSize();
            fileInfo.resetCursor();
        }
    } catch (const std::exception &e) {
        return res.status(400).json({{"error", e.what()}});
    }
    next();
}