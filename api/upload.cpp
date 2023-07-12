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

void parseATag(xmlNode *child, std::vector<size_t> &links,
               const Path &fileName) {
    xmlChar *href = xmlGetProp(child, (const xmlChar *)"href");
    if (href) {
        std::string hrefStr = xmlCharPtrToString(href);
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
                offset = tagsWeight[tag];
            if (tag == "a") parseATag(child, links, fileName);
            htmlNodeRecursive(child, weight + offset, wordExist,
                              wordWeightAndTotalByFile, fileName, links);
        }
    }
}

void parseHtmlFile(
    StreamFile &fileInfo,
    std::unordered_map<Word, Frequency> &numberOfFilesContainWord,
    const std::string &path) {
    char *buffer = new char[fileInfo.size];
    size_t bytes = fileInfo.read(buffer, fileInfo.size);
    std::pair<std::unordered_map<Word, Weight>, size_t>
        wordWeightAndTotalByFile;
    HtmlParser parser(buffer, fileInfo.size);
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
        std::cout << "Updating content for: " << fileInfo.name << std::endl;
        Sqlite::update("files",
                       "path = '" + path +
                           "', size = " + std::to_string(fileInfo.size) +
                           ", mimetype = 'text/html', numberOfWords = " +
                           std::to_string(wordWeightAndTotalByFile.second),
                       "name = '" + fileInfo.name + "'");
    } else {
        std::cout << "Inserting content for: " << fileInfo.name << std::endl;
        Sqlite::insert(
            "files", "name, path, size, mimetype, numberOfWords",
            std::string("'" + fileInfo.name + "', '" + path + "', " +
                        std::to_string(fileInfo.size) + ", 'text/html', " +
                        std::to_string(wordWeightAndTotalByFile.second))
                .c_str());
        fileID = Sqlite::lastInsertRowId();
    }
    // Sqlite::insert("CSRPageRankMatrix", "fr, t, value",
    //                std::string(std::to_string(fileID) + ", " +
    //                            std::to_string(fileID) + ", 1")
    //                    .c_str());
    for (auto &link : links) {
        Sqlite::insert("CSRPageRankMatrix", "fr, t, value",
                       std::string(std::to_string(fileID) + ", " +
                                   std::to_string(link) + ", 1")
                           .c_str());
    }
    for (auto &[word, weight] : wordWeightAndTotalByFile.first) {
        Sqlite::insert("wordsWeightByFile", "fileId, word, weight",
                       std::string(std::to_string(fileID) + ", '" + word +
                                   "', " + std::to_string(weight))
                           .c_str());
    }
    delete[] buffer;
}

const double damping_factor = 0.85;  // Damping factor for PageRank algorithm
const double convergence_threshold =
    0.0001;  // Convergence threshold for stopping iterations
const int max_iterations =
    100;  // Maximum number of iterations for PageRank algorithm
void runPageRank() {
    // TODO: implement
    double N = 0;
    Sqlite::exec(
        "SELECT COUNT(id) FROM files",
        [](void *data, int argc, char **argv, char **azColName) -> int {
            auto &N = *(double *)data;
            N = std::stod(argv[0]);
            return 0;
        },
        &N);
    std::vector<double> scores(N, 1.0 / N);
    for (int it = 0; it < max_iterations; ++it) {
        std::vector<double> newScores(N, 0);
        for (int j = 0; j < N; ++j) {
            // p = page[j]
            std::vector<int> ins;
            Sqlite::select(
                "CSRPageRankMatrix", "fr", "t = " + std::to_string(j + 1),
                [](void *data, int argc, char **argv, char **azColName) -> int {
                    auto &ins = *(std::vector<int> *)data;
                    ins.push_back(atoi(argv[0]));
                    return 0;
                },
                &ins);
            double sum = 0;
            for (int k = 0; k < ins.size(); ++k) {
                int nbOut = 1;
                Sqlite::select(
                    "CSRPageRankMatrix", "COUNT(t)",
                    "fr = " + std::to_string(ins[k]),
                    [](void *data, int argc, char **argv,
                       char **azColName) -> int {
                        auto &nbOut = *(int *)data;
                        nbOut = atoi(argv[0]);
                        return 0;
                    },
                    &nbOut);
                sum += scores[ins[k] - 1] / nbOut;  // -1 because id start at 1
            }
            double pr = (1 - damping_factor) / N + damping_factor * sum;
            newScores[j] = pr;
        }
        double diff = 0;
        for (int j = 0; j < N; ++j) diff += std::abs(newScores[j] - scores[j]);
        std::cout << "Iteration " << it << " diff: " << diff << std::endl;
        scores = newScores;
        if (diff < convergence_threshold) break;
    }
    for (int i = 0; i < N; ++i) {
        Sqlite::update(
            "files", "pageRank = " + std::to_string(scores[i]),
            "id = " + std::to_string(i + 1));  // +1 because id start at 1
    }
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
                parseHtmlFile(fileInfo, numberOfFilesContainWord, path);
            }
            file.close();
            fileInfo.moveFile("./app/dist/" + path);
            fileInfo.close();
        }
        for (auto &[word, freq] : numberOfFilesContainWord)
            updateWordsFileContain(word, freq);
        runPageRank();
        Sqlite::commitTransaction();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        Sqlite::rollbackTransaction();
    }
    // Redirect to upload
    res.redirect("/upload");
}