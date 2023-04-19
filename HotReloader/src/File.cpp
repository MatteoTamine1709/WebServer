#include "File.h"
#include <sys/stat.h>
#include <iostream>
#include <filesystem>

File::File(std::string path): m_path(path) {
    struct tm  tm;
    struct stat attrib;
    stat(path.c_str(), &attrib);
    m_lastUpdatedTime = attrib.st_mtime;
}

bool File::isUpdated() {
    struct tm  tm;
    struct stat attrib;
    stat(m_path.c_str(), &attrib);
    time_t lastUpdatedTime = attrib.st_mtime;
    if (lastUpdatedTime != m_lastUpdatedTime) {
        m_lastUpdatedTime = lastUpdatedTime;
        return true;
    }
    return false;
}

void File::compile() {
    std::cout << "Compiling " << m_path << std::endl;
    std::string outputFile = m_path.substr(0, m_path.find_last_of(".")) + ".so";
    std::string command = "g++ -shared -fPIC " + m_path + " -o " + outputFile + " -std=c++20" + " -I../";
    system(command.c_str());
}

std::string File::toAbsolutePath() {
    return std::filesystem::absolute(m_path).string();
}

std::ostream& operator<<(std::ostream &out, File &file) {
    out << file.m_path << " " << ctime(&file.m_lastUpdatedTime);
    return out;
}