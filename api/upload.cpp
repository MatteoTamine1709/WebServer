#include <fstream>
#include <iostream>
#include <unordered_set>

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
typedef double Weight;
typedef size_t Frequency;
typedef std::string Tag;

std::unordered_map<Tag, Weight> tagsWeight = {
    {"h1", 1.5}, {"h2", 1},       {"h3", 0.8},     {"h4", 0.6}, {"h5", 0.5},
    {"h6", 0.4}, {"title", 2},    {"strong", 0.2}, {"i", 0.1},  {"b", 0.2},
    {"em", 0.1}, {"header", 0.4}, {"footer", 0.2}};

std::string xmlCharPtrToString(const xmlChar *xmlCharPtr) {
    std::string str((const char *)xmlCharPtr);
    return str;
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
            std::string path = "./download/" + fileInfo.name;
            std::ofstream file("./app/dist/" + path, std::ios::binary);
            if (utils::endsWith(fileInfo.name, ".xml") ||
                utils::endsWith(fileInfo.name, ".html") ||
                utils::endsWith(fileInfo.name, ".xhtml")) {
                char *buffer = new char[fileInfo.size];
                size_t bytes = fileInfo.read(buffer, fileInfo.size);
                std::pair<std::unordered_map<Word, Weight>, size_t>
                    wordWeightAndTotalByFile;
                HtmlParser parser(buffer, fileInfo.size);
                xmlNode *root = parser.getRoot();
                if (!root) {
                    std::cerr << "Failed to parse file: " << fileInfo.name
                              << std::endl;
                    continue;
                }
                std::unordered_set<Word> wordExist;
                std::function<void(xmlNode *, Weight)> recursive =
                    [&](xmlNode *node, Weight weight) {
                        for (xmlNode *child = node->children; child;
                             child = child->next) {
                            // Dont include script and style tags
                            if (child->type == XML_ELEMENT_NODE &&
                                (std::string((const char *)child->name) ==
                                     std::string("script") ||
                                 std::string((const char *)child->name) ==
                                     std::string("style")))
                                continue;
                            if (child->type == XML_TEXT_NODE ||
                                child->type == XML_CDATA_SECTION_NODE) {
                                std::string content =
                                    std::string((const char *)child->content);
                                for (auto token : Lexer(content)) {
                                    if (wordWeightAndTotalByFile.first.find(
                                            token) ==
                                        wordWeightAndTotalByFile.first.end())
                                        wordWeightAndTotalByFile.first[token] =
                                            weight;
                                    else
                                        wordWeightAndTotalByFile.first[token] +=
                                            weight;
                                    wordWeightAndTotalByFile.second++;
                                    wordExist.insert(token);
                                }
                            } else if (child->type == XML_ELEMENT_NODE) {
                                std::string tag =
                                    xmlCharPtrToString(child->name);
                                if (tagsWeight.find(tag) != tagsWeight.end())
                                    weight += tagsWeight[tag];
                                recursive(child, weight);
                            }
                        }
                    };
                recursive(root, 1);
                for (auto &word : wordExist) {
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
                                std::to_string(wordWeightAndTotalByFile.second))
                        .c_str());
                sqlite3_int64 fileID = Sqlite::lastInsertRowId();
                for (auto &[word, weight] : wordWeightAndTotalByFile.first) {
                    Sqlite::insert(
                        "wordsWeightByFile", "fileId, word, weight",
                        std::string(std::to_string(fileID) + ", '" + word +
                                    "', " + std::to_string(weight))
                            .c_str());
                }
                delete[] buffer;
            }
            file.close();
            fileInfo.moveFile("./app/dist/" + path);
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