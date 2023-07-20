#ifndef PARSE_HTML_FILES_H
#define PARSE_HTML_FILES_H

#include "src/Request.h"
#include "types.h"

const std::unordered_map<Tag, Weight> tagsWeight = {
    {"h1", 1.5}, {"h2", 1},       {"h3", 0.8},     {"h4", 0.6}, {"h5", 0.5},
    {"h6", 0.4}, {"title", 2},    {"strong", 0.2}, {"i", 0.1},  {"b", 0.2},
    {"em", 0.1}, {"header", 0.4}, {"footer", 0.2}};

void parseATag(xmlNode *child, std::vector<size_t> &links,
               const Path &fileName) {
    xmlChar *href = xmlGetProp(child, (const xmlChar *)"href");
    if (href) {
        std::string hrefStr = (const char *)href;
        if (hrefStr.find("http") == std::string::npos &&
            hrefStr.find("https") == std::string::npos) {
            // We have a path to a file that will end up in the
            // database
            hrefStr = hrefStr.substr(0, hrefStr.find("#"));
            if (hrefStr.empty()) return;
            fs::path linkFileName = hrefStr;
            if (linkFileName.extension() != ".html" &&
                linkFileName.extension() != ".xhtml" &&
                linkFileName.extension() != ".xml")
                return;
            // If it is a relative path, resolve it compared to the
            // fileName EX: fileName =
            // htmx/htmx.org/migration-guide/index.html linkFileName
            // = ../docs/index.html newFileName =
            // htmx/htmx.org/docs/index.html
            //
            // fileName =
            // htmx/htmx.org/headers/hx-push-url/index.html
            // linkFileName = ../../essays/index.html
            // newFileName = htmx/htmx.org/essays/index.html
            if (linkFileName.is_relative()) {
                fs::path newFileName =
                    fs::path(fileName).parent_path() / linkFileName;
                hrefStr = newFileName.string();
            }
            // Go fr
            // htmx/htmx.org/migration-guide/../index.html
            // to htmx/htmx.org/index.html
            hrefStr = fs::weakly_canonical(hrefStr).string();
            // Go fr
            // htmx/htmx.org/index.html#installation
            // to htmx/htmx.org/index.html

            size_t fileID = 0;
            bool found = false;
            // Check if file already exist
            struct CallbackData {
                size_t *fileID;
                bool *found;
            };
            auto callback = [](void *data, int argc, char **argv,
                               char **azColName) -> int {
                CallbackData *callbackData = (CallbackData *)data;
                *callbackData->fileID = atoi(argv[0]);
                *callbackData->found = true;
                return 0;
            };

            CallbackData callbackData = {&fileID, &found};
            Sqlite::select("files", "id", "name = '" + hrefStr + "'", callback,
                           &callbackData);
            if (!found) {
                // File does not exist, insert it
                Sqlite::insert("files", "name", "'" + hrefStr + "'");
                fileID = Sqlite::lastInsertRowId();
            }
            links.push_back(fileID);
        }
    }
}

void htmlNodeRecursive(xmlNode *node, Weight weight,
                       std::unordered_set<Word> &wordExist,
                       std::pair<std::unordered_map<Word, Weight>, size_t>
                           &wordWeightAndTotalByFile,
                       const Path &fileName, std::vector<size_t> &links) {
    for (xmlNode *child = node->children; child; child = child->next) {
        // Dont include script and style tags
        if (child->type == XML_ELEMENT_NODE &&
            (std::string((const char *)child->name) == std::string("script") ||
             std::string((const char *)child->name) == std::string("style")))
            continue;
        if (child->type == XML_TEXT_NODE ||
            child->type == XML_CDATA_SECTION_NODE) {
            std::string content = std::string((const char *)child->content);
            for (auto token : Lexer(content)) {
                if (wordWeightAndTotalByFile.first.find(token) ==
                    wordWeightAndTotalByFile.first.end())
                    wordWeightAndTotalByFile.first[token] = weight;
                else
                    wordWeightAndTotalByFile.first[token] += weight;
                wordWeightAndTotalByFile.second++;
                wordExist.insert(token);
            }
        } else if (child->type == XML_ELEMENT_NODE) {
            Weight offset = 0;
            std::string tag = (const char *)child->name;
            if (tagsWeight.find(tag) != tagsWeight.end())
                offset = tagsWeight.at(tag);
            if (tag == "a") parseATag(child, links, fileName);
            htmlNodeRecursive(child, weight + offset, wordExist,
                              wordWeightAndTotalByFile, fileName, links);
        }
    }
}

void parseHtmlFile(
    StreamFile &fileInfo,
    std::unordered_map<Word, Frequency> &numberOfFilesContainWord,
    const std::string &downloadPath) {
    std::pair<std::unordered_map<Word, Weight>, size_t>
        wordWeightAndTotalByFile;
    HtmlParser parser(fileInfo.getPath());
    xmlNode *root = parser.getRoot();
    if (!root) {
        std::cerr << "Failed to parse file: " << fileInfo.name << std::endl;
        return;
    }
    std::unordered_set<Word> wordExist;
    std::vector<size_t> links;
    // std::cout << "In page: " << fileInfo.name << std::endl;
    htmlNodeRecursive(root, 1, wordExist, wordWeightAndTotalByFile,
                      fileInfo.name, links);
    for (auto &word : wordExist) {
        if (numberOfFilesContainWord.find(word) ==
            numberOfFilesContainWord.end())
            numberOfFilesContainWord[word] = 1;
        else
            numberOfFilesContainWord[word]++;
    }
    // Verify if file already exist and if so, update it
    bool found = false;
    size_t fileID = 0;
    struct CallbackData {
        size_t *fileID;
        bool *found;
    };

    auto callback = [](void *data, int argc, char **argv,
                       char **azColName) -> int {
        CallbackData *callbackData = (CallbackData *)data;
        *callbackData->fileID = atoi(argv[0]);
        *callbackData->found = true;
        return 0;
    };

    CallbackData callbackData = {&fileID, &found};
    Sqlite::select("files", "id", "name = '" + fileInfo.name + "'", callback,
                   &callbackData);
    if (found) {
        Sqlite::update("files",
                       "path = '" + downloadPath +
                           "', size = " + std::to_string(fileInfo.size) +
                           ", mimetype = 'text/html', numberOfWords = " +
                           std::to_string(wordWeightAndTotalByFile.second),
                       "name = '" + fileInfo.name + "'");
    } else {
        Sqlite::insert(
            "files", "name, path, size, mimetype, numberOfWords",
            std::string("'" + fileInfo.name + "', '" + downloadPath + "', " +
                        std::to_string(fileInfo.size) + ", 'text/html', " +
                        std::to_string(wordWeightAndTotalByFile.second))
                .c_str());
        fileID = Sqlite::lastInsertRowId();
    }
    // Sqlite::insert("CSRPageRankMatrix", "fr, t",
    //                std::string(std::to_string(fileID) + ", " +
    //                            std::to_string(fileID))
    //                    .c_str());
    for (auto &link : links) {
        Sqlite::insert(
            "CSRPageRankMatrix", "fr, t",
            std::string(std::to_string(fileID) + ", " + std::to_string(link))
                .c_str());
    }
    for (auto &[word, weight] : wordWeightAndTotalByFile.first) {
        Sqlite::insert("wordsWeightByFile", "fileId, word, weight",
                       std::string(std::to_string(fileID) + ", '" + word +
                                   "', " + std::to_string(weight))
                           .c_str());
    }
}

#endif  // PARSE_HTML_FILES_H