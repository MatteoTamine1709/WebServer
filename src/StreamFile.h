#ifndef STREAM_FILE_H
#define STREAM_FILE_H

#include <spdlog/spdlog.h>
#include <stdio.h>
#include <unistd.h>

#include <array>
#include <filesystem>
#include <iostream>
#include <string>

#include "Constants.h"

namespace fs = std::filesystem;
typedef struct StreamFile_s {
    std::string name{};
    std::string mimetype{};
    size_t size = 0;

    StreamFile_s() = default;

    bool create() {
        std::string tmptmpname = "/tmp/http-tmp-" + std::to_string(rand()) +
                                 "-" + std::to_string(time(NULL)) + "-XXXXXX";
        int fd = mkstemp(&tmptmpname[0]);
        m_path = tmptmpname;
        if (fd == -1) {
            SPDLOG_ERROR("Failed to create tmp file: {}", m_path);
            return false;
        }
        m_file = fdopen(fd, "w+");
        if (m_file == NULL) {
            SPDLOG_ERROR("Failed to open file: {}", m_path);
            return false;
        }
        return true;
    }

    bool open() {
        m_file = fopen(m_path.c_str(), "a+");
        if (m_file == NULL) {
            SPDLOG_ERROR("Failed to open file: {}", m_path);
            return false;
        }
        return true;
    }

    void close() {
        if (m_file != NULL) {
            fclose(m_file);
            m_file = NULL;
        }
    }

    StreamFile_s(const std::string &path) : m_path(path) {
        fs::create_directories(fs::path(path).parent_path());
        m_file = fopen(m_path.c_str(), "w+");
        if (m_file == NULL) {
            SPDLOG_ERROR("Failed to open file: {}", m_path);
            return;
        }
    }

    // Copy constructor
    StreamFile_s(const StreamFile_s &other) {
        if (other.m_file == NULL) return;
        m_path = other.m_path;
        m_file = fopen(m_path.c_str(), "a+");
        if (m_file == NULL) SPDLOG_ERROR("Failed to open file: {}", m_path);
        name = other.name;
        mimetype = other.mimetype;
        size = other.size;
    }

    // Move constructor
    StreamFile_s(StreamFile_s &&other) {
        if (other.m_file == NULL) return;
        m_path = other.m_path;
        m_file = other.m_file;
        name = other.name;
        mimetype = other.mimetype;
        size = other.size;
        other.m_file = NULL;
    }

    // Copy assignment operator
    StreamFile_s &operator=(const StreamFile_s &other) {
        if (other.m_file == NULL) return *this;
        m_path = other.m_path;
        fs::create_directories(fs::path(m_path).parent_path());
        m_file = fopen(m_path.c_str(), "a+");
        if (m_file == NULL) SPDLOG_ERROR("Failed to open file: {}", m_path);
        name = other.name;
        mimetype = other.mimetype;
        size = other.size;
        return *this;
    }

    ~StreamFile_s() {
        if (m_file != NULL) {
            fclose(m_file);
            m_file = NULL;
        }
    }

    std::string read() {
        std::string s;
        if (m_file != NULL) {
            std::array<char, 4 * ONE_MEGABYTE> buffer;
            size_t byte =
                fread(buffer.data(), sizeof(char), buffer.size(), m_file);
            s = std::string(buffer.data(), byte);
        }
        return s;
    }

    std::string readLine() {
        char *buffer = NULL;
        size_t bytes = 0;
        if (m_file != NULL) {
            size_t b = getline(&buffer, &bytes, m_file);
            return std::string(std::move(buffer), b);
        }
        return "";
    }

    size_t read(char *buffer, int bytes) {
        if (m_file != NULL) return fread(buffer, sizeof(char), bytes, m_file);
        return 0;
    }

    size_t write(const std::string &s) {
        if (m_file == NULL) return 0;
        return fwrite(s.data(), sizeof(char), s.size(), m_file);
    }

    size_t write(const char *s, size_t bytes) {
        if (m_file == NULL) return 0;
        return fwrite(s, sizeof(char), bytes, m_file);
    }

    bool isOpen() const { return m_file != NULL; }

    bool isEnd() const {
        if (m_file == NULL) return true;
        return feof(m_file);
    }

    size_t getSize() {
        if (m_file == NULL) return 0;
        size_t pos = ftell(m_file);
        fseek(m_file, 0, SEEK_END);
        size_t size = ftell(m_file);
        fseek(m_file, pos, SEEK_SET);
        return size;
    }

    size_t getCursor() const {
        if (m_file == NULL) return -1;
        return ftell(m_file);
    }

    int setCursor(size_t pos) {
        if (pos > size) return -1;
        if (m_file == NULL) return -1;
        return fseek(m_file, pos, SEEK_SET);
    }

    int resetCursor() { return setCursor(0); }

    bool moveFile(const std::string &path) {
        if (m_file == NULL) return false;
        close();
        fs::create_directories(fs::path(path).parent_path());
        if (rename(m_path.c_str(), path.c_str()) == 0) {
            m_path = path;
            open();
            return true;
        }
        return false;
    }

   private:
    std::string m_path = "/tmp/http-tmp-XXXXXX";
    FILE *m_file = nullptr;
} StreamFile;

#endif