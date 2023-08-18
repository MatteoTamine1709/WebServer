#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <unordered_set>

#include "../Lexer.h"
#include "HtmlParser.h"
#include "PageRank.h"
#include "ParseHtmlFiles.h"
#include "src/Request.h"
#include "src/Response.h"
#include "src/Sqlite.h"

extern "C" {
void get(Request &req, Response &res);
void post(Request &req, Response &res);
}

void get(Request &req, Response &res) {
    res.sendFile("./app/dist/upload.html");
}

void updateWordsFileContain(const Word &word, const Frequency &freq) {
    // If word already exist, update it
    bool found = false;
    struct CallbackData {
        bool *found;
        Word word;
        Frequency freq;
    };
    auto callback = [](void *data, int argc, char **argv,
                       char **azColName) -> int {
        CallbackData *callbackData = (CallbackData *)data;
        *callbackData->found = true;
        Sqlite::update("wordsFileContain",
                       "numberOfFileContains = " +
                           std::to_string(atoi(argv[1]) + callbackData->freq),
                       "word = '" + callbackData->word + "'");
        return 0;
    };
    CallbackData callbackData = {&found, word, freq};
    Sqlite::select("wordsFileContain", "word, numberOfFileContains",
                   "word = '" + word + "'", callback, &callbackData);
    if (found) return;
    Sqlite::insert(
        "wordsFileContain", "word, numberOfFileContains",
        std::string("'" + word + "', " + std::to_string(freq)).c_str());
}

void post(Request &req, Response &res) {
    std::cout << "# files:" << req.files.size() << std::endl;

    try {
        Sqlite::beginTransaction();
        std::unordered_map<Word, Frequency> numberOfFilesContainWord;
        size_t idx = 0;
        for (auto &[_, fileInfo] : req.files) {
            std::cout << "File " << idx++ << ": " << fileInfo.name << "\n";
            fileInfo.open();
            if (("./app/dist/download/" + fileInfo.name).find_last_of("/") !=
                std::string::npos)
                fs::create_directories(
                    "./app/dist/download/" +
                    fileInfo.name.substr(0, fileInfo.name.find_last_of("/")) +
                    "/");
            std::string downloadPath = "./download/" + fileInfo.name;
            if (utils::endsWith(fileInfo.name, ".xml") ||
                utils::endsWith(fileInfo.name, ".html") ||
                utils::endsWith(fileInfo.name, ".xhtml")) {
                parseHtmlFile(fileInfo, numberOfFilesContainWord, downloadPath);
            }
            fileInfo.moveFile("./app/dist/" + downloadPath);
            fileInfo.close();
        }
        for (auto &[word, freq] : numberOfFilesContainWord)
            updateWordsFileContain(word, freq);
        Sqlite::commitTransaction();
        runPageRank();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        Sqlite::rollbackTransaction();
    }
    // Redirect to upload
    res.redirect("/upload");
}