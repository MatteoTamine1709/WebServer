#include "../Sqlite.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream>

sqlite3* Sqlite::m_db = nullptr;

Sqlite::~Sqlite() { close(); }

bool Sqlite::open(const char* dbPath) {
    if (m_db != nullptr) return false;

    int rc = sqlite3_open(dbPath, &m_db);
    if (rc != SQLITE_OK) return false;

    return true;
}

bool Sqlite::close() {
    if (m_db == nullptr) return false;

    int rc = sqlite3_close(m_db);
    if (rc != SQLITE_OK) return false;

    m_db = nullptr;
    return true;
}

bool Sqlite::exec(const char* sql) {
    if (m_db == nullptr) return false;

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool Sqlite::exec(const char* sql, Callback callback, void* data) {
    if (m_db == nullptr) return false;

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql, callback, data, &errMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool Sqlite::beginTransaction() {
    if (m_db == nullptr) return false;

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, "BEGIN TRANSACTION", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool Sqlite::commitTransaction() {
    if (m_db == nullptr) return false;

    char* errMsg = nullptr;
    int rc =
        sqlite3_exec(m_db, "COMMIT TRANSACTION", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool Sqlite::rollbackTransaction() {
    if (m_db == nullptr) return false;

    char* errMsg = nullptr;
    int rc =
        sqlite3_exec(m_db, "ROLLBACK TRANSACTION", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool Sqlite::isTableExist(const char* tableName) {
    if (m_db == nullptr) return false;

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, "BEGIN TRANSACTION", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    char sql[256];
    sprintf(
        sql,
        "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='%s'",
        tableName);
    int count = 0;
    rc = sqlite3_exec(
        m_db, sql,
        [](void* data, int argc, char** argv, char** azColName) {
            int* count = (int*)data;
            *count = atoi(argv[0]);
            return 0;
        },
        &count, &errMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    rc = sqlite3_exec(m_db, "COMMIT TRANSACTION", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    return count > 0;
}

bool Sqlite::insert(const char* tableName, const char* columns,
                    const char* values) {
    if (m_db == nullptr) return false;

    char sql[256];
    sprintf(sql, "INSERT INTO %s (%s) VALUES (%s)", tableName, columns, values);
    return exec(sql);
}

bool Sqlite::update(const char* tableName, const char* set, const char* where) {
    if (m_db == nullptr) return false;

    char sql[256];
    sprintf(sql, "UPDATE %s SET %s WHERE %s", tableName, set, where);
    return exec(sql);
}

bool Sqlite::remove(const char* tableName, const char* where) {
    if (m_db == nullptr) return false;

    char sql[256];
    sprintf(sql, "DELETE FROM %s WHERE %s", tableName, where);
    return exec(sql);
}

bool Sqlite::select(const char* tableName, const char* columns,
                    const char* where, Callback callback, void* data) {
    if (m_db == nullptr) return false;

    char sql[256];
    sprintf(sql, "SELECT %s FROM %s WHERE %s", columns, tableName,
            where ? where : "1");
    return exec(sql, callback, data);
}
