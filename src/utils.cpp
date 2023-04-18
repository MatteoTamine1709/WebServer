#include "utils.h"
#include "HttpRequestHeader.h"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>

#include <spdlog/spdlog.h>

namespace utils {
    std::unordered_map<std::string, std::string> mimeMap = {};
    std::vector<std::string> split(const std::string& str, const std::vector<std::string>& delimiters) {
        std::vector<std::string> tokens;
        std::string::size_type start = 0;
        std::string::size_type end = 0;
        while (end != std::string::npos) {
            end = std::string::npos;
            for (const auto& delimiter : delimiters) {
                std::string::size_type pos = str.find(delimiter, start);
                if (pos != std::string::npos && (pos < end || end == std::string::npos)) {
                    end = pos;
                }
            }
            tokens.push_back(trim(str.substr(start, end - start)));
            start = end + 1;
        }
        return tokens;
    }

    std::string join(const std::vector<std::string>& tokens, const std::string& delimiter) {
        std::string result;
        for (const auto& token : tokens)
            result += token + delimiter;
        return result;
    }

    std::string trim(const std::string& str) {
        std::string result = str;
        result.erase(0, result.find_first_not_of(' '));
        result.erase(result.find_last_not_of(' ') + 1);
        return result;
    }

    std::string toLower(const std::string& str) {
        std::string result = str;
        for (auto &c : result) {
            c = tolower(c);
        }
        return result;
    }

    std::string toUpper(const std::string& str) {
        std::string result = str;
        for (auto &c : result)
            c = toupper(c);
        return result;
    }

    std::string replace(const std::string& str, const std::string& from, const std::string& to) {
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
        if (mimeMap.find(extension) == mimeMap.end())
            return "application/octet-stream";
        return mimeMap[extension];
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
        strftime (buffer, 80, "%a, %d %b %Y %H:%M:%S", timeinfo);
        return std::string(buffer);
    }

    void initializeMimeMap() {
        if (mimeMap.size() == 0) {
            spdlog::debug("Initializing mime map");
            std::ifstream file("./utils/mime.txt");
            if (!file.is_open()) {
                spdlog::error("utils/mime.txt not found");
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
}