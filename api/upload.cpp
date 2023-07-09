#include <fstream>
#include <iostream>

#include "Lexer.h"
#include "XmlParser.h"
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

typedef std::string Path;
typedef std::string Word;
typedef size_t Frequency;

void post(Request &req, Response &res) {
    std::cout << "# files:" << req.files.size() << std::endl;

    try {
        Sqlite::beginTransaction();
        std::unordered_map<Word, Frequency> numberOfFilesContainWord;
        size_t idx = 0;
        for (auto &[_, fileInfo] : req.files) {
            std::cout << "File " << idx++ << ": " << fileInfo.name << std::endl;
            fileInfo.open();
            if (("./app/dist/download/" + fileInfo.name).find_last_of("/") !=
                std::string::npos)
                fs::create_directories(
                    "./app/dist/download/" +
                    fileInfo.name.substr(0, fileInfo.name.find_last_of("/")) +
                    "/");
            std::string path = "./download/" + fileInfo.name;
            std::ofstream file("./app/dist/" + path, std::ios::binary);
            char *buffer = new char[fileInfo.size];
            size_t bytes = fileInfo.read(buffer, fileInfo.size);
            file.write(buffer, bytes);
            if (utils::endsWith(fileInfo.name, ".xml") ||
                utils::endsWith(fileInfo.name, ".html")) {
                std::pair<std::unordered_map<Word, Frequency>, size_t>
                    wordFreqAndTotalByFile;
                HtmlParser parser(buffer, fileInfo.size);
                std::string content(parser.extractContent());
                for (auto token : Lexer(content)) {
                    if (wordFreqAndTotalByFile.first.find(token) ==
                        wordFreqAndTotalByFile.first.end())
                        wordFreqAndTotalByFile.first[token] = 1;
                    else
                        wordFreqAndTotalByFile.first[token]++;
                    wordFreqAndTotalByFile.second++;
                }
                for (auto &[word, freq] : wordFreqAndTotalByFile.first) {
                    if (numberOfFilesContainWord.find(word) ==
                        numberOfFilesContainWord.end())
                        numberOfFilesContainWord[word] = 1;
                    else
                        numberOfFilesContainWord[word]++;
                }
                Sqlite::insert(
                    "files", "name, path, size, mimetype, numberOfWords",
                    std::string("'" + fileInfo.name + "', '" + path + "', " +
                                std::to_string(fileInfo.size) +
                                ", 'text/html', " +
                                std::to_string(wordFreqAndTotalByFile.second))
                        .c_str());
                sqlite3_int64 fileID = Sqlite::lastInsertRowId();
                for (auto &[word, freq] : wordFreqAndTotalByFile.first) {
                    Sqlite::insert(
                        "wordsFreqByFile", "fileId, word, freq",
                        std::string(std::to_string(fileID) + ", '" + word +
                                    "', " + std::to_string(freq))
                            .c_str());
                }
            }

            delete[] buffer;
            file.close();
            fileInfo.close();
        }
        for (auto &[word, freq] : numberOfFilesContainWord) {
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
                Sqlite::update(
                    "wordsFileContain",
                    "numberOfFileContains = " +
                        std::to_string(atoi(argv[1]) + callbackData->freq),
                    "word = '" + callbackData->word + "'");
                return 0;
            };
            CallbackData callbackData = {&found, word, freq};
            Sqlite::select("wordsFileContain", "word, numberOfFileContains",
                           "word = '" + word + "'", callback, &callbackData);
            if (found) continue;
            Sqlite::insert(
                "wordsFileContain", "word, numberOfFileContains",
                std::string("'" + word + "', " + std::to_string(freq)).c_str());
        }
        Sqlite::commitTransaction();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        Sqlite::rollbackTransaction();
    }
    // Redirect to upload
    res.redirect("/upload");
}