#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <dirent.h>
#include "File.h"

std::unordered_map<std::string, File> files = {};
void loadFiles(std::string path) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(path.c_str());
    if (dir == nullptr) {
        std::cout << "Unable to open directory." << std::endl;
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            if (std::string(entry->d_name).find(".cpp") == std::string::npos) 
                continue;
            if (files.find(path + std::string(entry->d_name)) == files.end())
                files[path + std::string(entry->d_name)] = File(path + std::string(entry->d_name));
        } else if (entry->d_type == DT_DIR) {
            if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
                continue;
            loadFiles(path + entry->d_name + "/");
        }
    }

    closedir(dir);
}