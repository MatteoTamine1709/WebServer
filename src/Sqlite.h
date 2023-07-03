#ifndef SQL_LITE_H
#define SQL_LITE_H

#include <sqlite3.h>

class Sqlite {
   public:
    typedef int (*Callback)(void*, int, char**, char**);
    Sqlite() = delete;
    ~Sqlite();

    static bool open(const char* dbPath);
    static bool close();

    static bool exec(const char* sql);
    static bool exec(const char* sql, Callback, void* data);

    static bool beginTransaction();
    static bool commitTransaction();
    static bool rollbackTransaction();

    static bool isTableExist(const char* tableName);

    static bool insert(const char* tableName, const char* columns,
                       const char* values);
    static bool update(const char* tableName, const char* set,
                       const char* where);
    static bool remove(const char* tableName, const char* where);

    static bool select(const char* tableName, const char* columns,
                       const char* where, Callback callback, void* data);

   private:
    static sqlite3* m_db;
};

#endif  // SQL_LITE_H