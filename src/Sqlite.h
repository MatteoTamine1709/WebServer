#ifndef SQL_LITE_H
#define SQL_LITE_H

#include <sqlite3.h>

#include <string>

class Sqlite {
   public:
    typedef int (*Callback)(void*, int, char**, char**);
    Sqlite() = delete;
    ~Sqlite();

    static bool open(const char* dbPath);
    static bool close();

    static bool exec(const std::string sql);
    static bool exec(const char* sql);
    static bool exec(const std::string sql, Callback, void* data);
    static bool exec(const char* sql, Callback, void* data);

    static bool beginTransaction();
    static bool commitTransaction();
    static bool rollbackTransaction();

    static bool isTableExist(const char* tableName);

    static bool insert(const std::string tableName, const std::string columns,
                       const std::string values);
    static bool insert(const char* tableName, const char* columns,
                       const char* values);

    static bool update(const std::string tableName, const std::string set,
                       const std::string where);
    static bool update(const char* tableName, const char* set,
                       const char* where);

    static bool remove(const std::string tableName, const std::string where);
    static bool remove(const char* tableName, const char* where);

    static bool select(const std::string tableName, const std::string columns,
                       const std::string where, Callback callback, void* data);
    static bool select(const char* tableName, const char* columns,
                       const char* where, Callback callback, void* data);

    static bool delete_(const std::string tableName, const std::string where);
    static bool delete_(const char* tableName, const char* where);

    static sqlite3_int64 lastInsertRowId() {
        return sqlite3_last_insert_rowid(m_db);
    }

    static sqlite3* getDB() { return m_db; }

   private:
    static sqlite3* m_db;
};

#endif  // SQL_LITE_H