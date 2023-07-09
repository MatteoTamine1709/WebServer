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

double tf(size_t wordFreq, size_t numberOfWords) {
    return (double)wordFreq / (double)numberOfWords;
}

double idf(size_t numberOfFiles, size_t numberOfFilesContainWord) {
    return log10(numberOfFiles /
                 (double)(std::max(numberOfFilesContainWord, 1UL)));
}

void get(Request &req, Response &res) {
    std::string query = req.query["query"];
    typedef std::string Path;
    typedef std::string Word;
    typedef double Score;
    typedef size_t Frequency;
    std::unordered_map<Path, Score> scores;
    size_t numberOfFiles = 0;
    Sqlite::exec(
        "SELECT COUNT(id) FROM files",
        [](void *data, int argc, char **argv, char **azColName) -> int {
            auto &numberOfFiles = *(size_t *)data;
            numberOfFiles = atoi(argv[0]);
            return 0;
        },
        &numberOfFiles);
    for (auto token : Lexer(query)) {
        std::cout << "Token: " << token << std::endl;
        std::unordered_map<Path, std::pair<Frequency, size_t>>
            tokensFreqAndTotalByFile;
        Sqlite::exec(
            "SELECT files.path, wordsFreqByFile.freq, files.numberOfWords "
            "FROM files "
            "INNER JOIN wordsFreqByFile ON files.id = wordsFreqByFile.fileId "
            "WHERE wordsFreqByFile.word = '" +
                token + "' ORDER BY wordsFreqByFile.freq ASC",
            [](void *data, int argc, char **argv, char **azColName) -> int {
                auto &tokensFreqAndTotalByFile =
                    *(std::unordered_map<Path, std::pair<Frequency, size_t>> *)
                        data;
                tokensFreqAndTotalByFile[argv[0]] = {atoi(argv[1]),
                                                     atoi(argv[2])};
                return 0;
            },
            &tokensFreqAndTotalByFile);
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
        for (auto &[path, freqAndTotal] : tokensFreqAndTotalByFile) {
            if (scores.find(path) == scores.end()) scores[path] = 0;
            scores[path] += tf(freqAndTotal.first, freqAndTotal.second) *
                            idf(numberOfFiles, numberOfFilesContainWord);
        }
    }
    std::vector<std::pair<Path, Score>> sortedScores;
    for (auto &[path, score] : scores) sortedScores.push_back({path, score});
    std::sort(sortedScores.begin(), sortedScores.end(),
              [](const std::pair<Path, Score> &a,
                 const std::pair<Path, Score> &b) -> bool {
                  return a.second > b.second;
              });
    std::vector<std::pair<Path, Score>> top10Scores;
    for (size_t i = 0; i < 10 && i < sortedScores.size(); i++)
        top10Scores.push_back(sortedScores[i]);
    HtmlBuilder html{};
    html.h3().text("Links").h3_();
    html.ul();
    for (const auto &[path, score] : top10Scores)
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