#ifndef STREAM_FILE_H
#define STREAM_FILE_H

#include <spdlog/spdlog.h>
#include <stdio.h>
#include <unistd.h>

#include <array>
#include <iostream>
#include <string>

#include "Constants.h"
typedef struct StreamFile_s {
    std::string name;
    std::string mimetype;
    size_t size;

    StreamFile_s() {
        // Generate random file name using mkdstemp
        char tmptmpname[] = "/tmp/http-tmp-XXXXXX";
        int fd = mkstemp(tmptmpname);
        m_path = tmptmpname;
        if (fd == -1) {
            spdlog::error("Failed to create tmp file: {}", m_path);
            return;
        }
        m_file = fdopen(fd, "w+");
        if (m_file == NULL) {
            spdlog::error("Failed to open file: {}", m_path);
            return;
        }
    }

    StreamFile_s(const std::string &path) : m_path(path) {
        m_file = fopen(m_path.c_str(), "w+");
        if (m_file == NULL) {
            spdlog::error("Failed to open file: {}", m_path);
            return;
        }
    }

    // Copy constructor
    StreamFile_s(const StreamFile_s &other) {
        // if (m_file != NULL) fclose(m_file);
        m_path = other.m_path;
        m_file = fopen(m_path.c_str(), "w+");
        name = other.name;
        mimetype = other.mimetype;
        size = other.size;
    }

    // Move constructor
    StreamFile_s(StreamFile_s &&other) {
        // if (m_file != NULL) fclose(m_file);
        m_path = other.m_path;
        m_file = other.m_file;
        name = other.name;
        mimetype = other.mimetype;
        size = other.size;
        other.m_file = NULL;
    }

    // Copy assignment operator
    StreamFile_s &operator=(const StreamFile_s &other) {
        // if (m_file != NULL) fclose(m_file);
        m_path = other.m_path;
        m_file = fopen(m_path.c_str(), "w+");
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

    size_t read(char *buffer, int bytes) {
        if (m_file != NULL) {
            return fread(buffer, sizeof(char), bytes, m_file);
        }
        return 0;
    }

    void write(const std::string &s) {
        if (m_file != NULL) {
            fwrite(s.data(), sizeof(char), s.size(), m_file);
        }
    }

    void write(const char *s, size_t bytes) {
        if (m_file != NULL) {
            std::cout << "Written " << fwrite(s, sizeof(char), bytes, m_file)
                      << std::endl;
        }
    }

    bool isOpen() const { return m_file != NULL; }

    bool isEnd() const {
        if (m_file == NULL) return true;
        return feof(m_file);
    }

    void reset() {
        if (m_file != NULL) {
            fseek(m_file, 0, SEEK_SET);
        }
    }

   private:
    std::string m_path = "/tmp/http-tmp-XXXXXX";
    FILE *m_file;
} StreamFile;

#endif