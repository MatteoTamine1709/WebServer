#include "utils.h"
#include "HttpRequestHeader.h"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>

#include <spdlog/spdlog.h>
#include <iostream>
#include <algorithm>
#include <sstream>

namespace utils {
    std::unordered_map<std::string, std::string> mimeMap = {};
    std::vector<std::string> split(const std::string& str, std::vector<std::string> delimiters) {
        std::vector<std::string> tokens;
        bool isEmptyDelimiter = false;
        for (int i = 0; i < delimiters.size(); i++)
            if (delimiters[i].length() == 0) {
                isEmptyDelimiter = true;
                break;
            }
        if (isEmptyDelimiter) {
            for (int i = 0; i < str.length(); i++)
                tokens.push_back(str.substr(i, 1));
            return tokens;
        }
        std::sort(delimiters.begin(), delimiters.end(), [](const std::string& a, const std::string& b) {
            return a.length() > b.length();
        });
        std::string current = str;
        while (current.length() > 0) {
            bool found = false;
            for (int i = 0; i < delimiters.size(); i++) {
                if (current.length() >= delimiters[i].length() && current.substr(0, delimiters[i].length()) == delimiters[i]) {
                    current = current.substr(delimiters[i].length());
                    found = true;
                    break;
                }
            }
            size_t pos = current.length();
            for (int i = 0; i < delimiters.size(); i++)
                if (current.find(delimiters[i]) < pos)
                    pos = current.find(delimiters[i]);
            if (pos == 0)
                continue;
            tokens.push_back(current.substr(0, pos));
            current = current.substr(pos);
        }
        if (tokens.size() == 0)
            tokens.push_back(str);
        return tokens;
    }

    std::string join(const std::vector<std::string>& tokens, const std::string& delimiter) {
        std::string result;
        for (int i = 0; i < tokens.size(); i++) {
            result += tokens[i];
            if (i < tokens.size() - 1)
                result += delimiter;
        }
        return result;
    }

    std::string trim(const std::string& str) {
        std::string result = str;
        while (result.length() > 0 && (result[0] == ' ' || result[0] == '\t' || result[0] == '\n'))
            result = result.substr(1);
        while (result.length() > 0 && (result[result.length() - 1] == ' ' || result[result.length() - 1] == '\t' || result[result.length() - 1] == '\n'))
            result = result.substr(0, result.length() - 1);
        return result;
    }

    std::string toLower(const std::string& str) {
        std::string result = str;
        for (auto &c : result)
            c = std::tolower(c);
        return result;
    }

    std::string toUpper(const std::string& str) {
        std::string result = str;
        for (auto &c : result)
            c = std::toupper(c);
        return result;
    }

    bool startsWith(const std::string& str, const std::string& prefix) {
        return str.find(prefix) == 0;
    }

    bool endsWith(const std::string& str, const std::string& suffix) {
        if (suffix.length() > str.length())
            return false;
        return str.rfind(suffix) == (str.length() - suffix.length());
    }

    std::string replace(const std::string& str, const std::string& from, const std::string& to) {
        if (from.empty())
            return str;
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        return result;
    }

    std::string getExtension(const std::string& path) {
        size_t pos = path.find_last_of('.');
        if (pos == std::string::npos)
            return "";
        return path.substr(pos + 1);
    }

    std::string getMimeType(const std::string& extension) {
        std::string ext = trim(toLower(extension));
        if (mimeMap.find(ext) == mimeMap.end())
            return "unknown";
        return mimeMap[ext];
    }

    std::string getContentType(const std::string& path) {
        std::string extension = getExtension(path);
        return getMimeType(extension);
    }

    std::string getDateTime() {
        time_t rawtime;
        time(&rawtime);
        struct tm *timeinfo = gmtime(&rawtime);

        char buffer [80];
        // Format : <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
        // Example: Tue, 15 Nov 1994 08:12:31 GMT
        strftime (buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
        return std::string(buffer);
    }

    void initializeMimeMap() {
        if (mimeMap.size() == 0) {
            SPDLOG_INFO("Initializing mime map");
            std::ifstream file("./utils/mime.txt");
            if (!file.is_open()) {
                SPDLOG_CRITICAL("utils/mime.txt not found");
                exit(1);
            }
            std::stringstream ss;
            ss << file.rdbuf();
            std::string content = ss.str();
            std::vector<std::string> lines = split(content, {"\n", ": "});
            for (int i = 0; i < lines.size(); i += 2)
                mimeMap[lines[i]] = lines[i + 1];
        }
    }

    std::string makeEtag(const std::string &filePath) {
        struct tm  tm;
        struct stat attrib;
        if (stat(filePath.c_str(), &attrib) == -1)
            return "";
        std::stringstream ss;
        ss << attrib.st_mtime << "-";
        ss << attrib.st_size << "-";
        ss << attrib.st_ino;
        return ss.str();
    }

    std::string getLastModified(const std::string &filePath) {
        struct tm  tm;
        struct stat attrib;
        if (stat(filePath.c_str(), &attrib) == -1)
            return "";
        gmtime_r(&(attrib.st_mtime), &tm);
        char buffer [80];
        strftime (buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", &tm);
        return std::string(buffer);
    }
}