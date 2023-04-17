#ifndef FILE_H
#define FILE_H

#include <string>
#include <unordered_map>


class File {
    public:
        File() = default;
        File(std::string path);

        bool isUpdated();
        void compile();
        std::string toAbsolutePath();

        // Operator
        friend std::ostream& operator<<(std::ostream &out, File &file);
    private:
        std::string m_path;
        time_t m_lastUpdatedTime;
};
extern std::unordered_map<std::string, File> files;

#endif  