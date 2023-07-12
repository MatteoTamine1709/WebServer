#include <fstream>
#include <iostream>
#include <sstream>

#define CSS_BUILDER_IMPLEMENTATION
#include "CssBuilder.h"
#define HTML_BUILDER_IMPLEMENTATION
#include "HtmlBuilder.h"
#include "Lexer.h"
#include "src/Request.h"
#include "src/Response.h"
#include "src/Sqlite.h"

extern "C" {
void get(Request &req, Response &res);
void post(Request &req, Response &res);
}

double tf(double wordWeight, size_t numberOfWords) {
    return wordWeight / (double)numberOfWords;
}

double idf(size_t numberOfFiles, size_t numberOfFilesContainWord) {
    return log(((double)(numberOfFiles - numberOfFilesContainWord) + 0.5) /
               (double)(numberOfFilesContainWord + 0.5)) +
           1;
}

const double K1 = 2;
const double b = 0.75;

void get(Request &req, Response &res) {
    std::string query = req.query["query"];
    typedef std::string Path;
    typedef std::string Word;
    typedef double Score;
    typedef double PageRank;
    typedef double Weight;
    std::unordered_map<Path, Score> scores;
    std::unordered_map<Path, PageRank> pageRanks;
    size_t N = 0;
    Sqlite::exec(
        "SELECT COUNT(id) FROM files",
        [](void *data, int argc, char **argv, char **azColName) -> int {
            auto &N = *(size_t *)data;
            N = atol(argv[0]);
            return 0;
        },
        &N);
    double averageNumberOfWords = 0;
    Sqlite::exec(
        "SELECT AVG(numberOfWords) FROM files",
        [](void *data, int argc, char **argv, char **azColName) -> int {
            if (argv[0] == NULL) return 0;
            auto &averageNumberOfWords = *(double *)data;
            averageNumberOfWords = std::stod(argv[0]);
            return 0;
        },
        &averageNumberOfWords);
    for (auto token : Lexer(query)) {
        std::cout << "Token: " << token << std::endl;
        struct Data {
            Weight weight;
            size_t numberOfWords;
            PageRank pageRank;
        };
        std::unordered_map<Path, Data> tokensWeightAndTotalByFile;
        Sqlite::exec(
            "SELECT files.path, wordsWeightByFile.weight, "
            "files.numberOfWords, wordsWeightByFile.word, levenshtein(word, '" +
                token +
                "') AS distance, files.pageRank"
                " FROM files "
                " INNER JOIN wordsWeightByFile ON files.id = "
                "wordsWeightByFile.fileId "
                "WHERE wordsWeightByFile.word IN (SELECT word FROM "
                "wordsFileContain WHERE levenshtein(word, '" +
                token + "') <= 2)",
            [](void *data, int argc, char **argv, char **azColName) -> int {
                auto &tokensWeightAndTotalByFile =
                    *(std::unordered_map<Path, Data> *)data;
                std::string path = argv[0];
                double weight = std::stod(argv[1]);
                size_t numberOfWords = atol(argv[2]);
                std::string word = argv[3];
                size_t distance = atoi(argv[4]);
                double pageRank = std::stod(argv[5]);
                if (tokensWeightAndTotalByFile.find(path) ==
                    tokensWeightAndTotalByFile.end())
                    tokensWeightAndTotalByFile[path] = {0, 0, 0};
                tokensWeightAndTotalByFile[path].numberOfWords = numberOfWords;
                tokensWeightAndTotalByFile[path].weight +=
                    weight * (1 - pow(distance, 2) / word.length());
                tokensWeightAndTotalByFile[path].pageRank = pageRank;
                return 0;
            },
            &tokensWeightAndTotalByFile);
        size_t numberOfFilesContainWord = 0;
        Sqlite::exec(
            "SELECT numberOfFileContains FROM wordsFileContain WHERE word = '" +
                token + "'",
            [](void *data, int argc, char **argv, char **azColName) -> int {
                auto &numberOfFilesContainWord = *(size_t *)data;
                numberOfFilesContainWord = atoi(argv[0]);
                return 0;
            },
            &numberOfFilesContainWord);
        for (auto &[path, weightAndTotal] : tokensWeightAndTotalByFile) {
            if (scores.find(path) == scores.end()) scores[path] = 0;
            double TF = tf(weightAndTotal.weight, weightAndTotal.numberOfWords);
            double IDF = idf(N, numberOfFilesContainWord);
            scores[path] +=
                IDF *
                ((TF * K1) / (TF + K1 * (1 - b + b * averageNumberOfWords)));
            pageRanks[path] = weightAndTotal.pageRank;
        }
    }
    std::vector<std::pair<Path, Score>> sortedScores;
    for (auto &[path, score] : scores)
        sortedScores.push_back({path, score * pageRanks[path]});
    std::sort(sortedScores.begin(), sortedScores.end(),
              [](const std::pair<Path, Score> &a,
                 const std::pair<Path, Score> &b) -> bool {
                  return a.second > b.second;
              });
    for (auto &[path, score] : sortedScores)
        std::cout << path << " --- " << scores[path] << ", " << pageRanks[path]
                  << std::endl;
    std::vector<std::pair<Path, Score>> top50Scores;
    for (size_t i = 0; i < 50 && i < sortedScores.size(); i++)
        top50Scores.push_back(sortedScores[i]);
    HtmlBuilder html{};
    html.h3().text("Links").h3_();
    html.ul();
    for (const auto &[path, score] : top50Scores)
        html.li()
            .a({
                {"href", path},
            })
            .text(path)
            .a_()
            .text(" --- " + std::to_string(score))
            .li_();
    html.ul_();
    res.send(html.toString());
}