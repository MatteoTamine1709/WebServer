#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <filesystem>

namespace utils {
    extern std::unordered_map<std::string, std::string> mimeMap;
    void initializeMimeMap();
    std::vector<std::string> split(const std::string& str, std::vector<std::string> delimiters);
    std::string join(const std::vector<std::string>& tokens, const std::string& delimiter);
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    bool startsWith(const std::string& str, const std::string& prefix);
    bool endsWith(const std::string& str, const std::string& suffix);
    std::string replace(const std::string& str, const std::string& from, const std::string& to);
    std::string getExtension(const std::string& path);
    std::string getMimeType(const std::string& extension);
    std::string getContentType(const std::string& path);
    std::string getDateTime();
    std::string makeEtag(const std::string& filePath);
    std::string getLastModified(const std::string& filePath);
    bool isRegexSubset(std::string regex1, std::string regex2);
    bool isPathInsideOtherPath(const std::filesystem::path& path1, const std::filesystem::path& path2);
}

#endif