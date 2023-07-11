#include <functional>

#include "LevenshteinDist.h"
#include "src/Sqlite.h"
#include "src/TcpServer.h"

void distance(sqlite3_context *context, int argc, sqlite3_value **argv) {
    if (argc != 2) {
        sqlite3_result_error(context, "Invalid number of arguments", -1);
        return;
    }
    if (sqlite3_value_type(argv[0]) != SQLITE_TEXT ||
        sqlite3_value_type(argv[1]) != SQLITE_TEXT) {
        sqlite3_result_error(context, "Invalid argument type", -1);
        return;
    }
    const char *s1 = (const char *)sqlite3_value_text(argv[0]);
    const char *s2 = (const char *)sqlite3_value_text(argv[1]);
    sqlite3_result_int(context, LevenshteinDist::distance(s1, s2));
}

int main(int, char **) {
    Sqlite::open("./db.sqlite3");
    Sqlite::exec(
        "CREATE TABLE IF NOT EXISTS files ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "path TEXT NOT NULL,"
        "size INTEGER NOT NULL,"
        "mimetype TEXT NOT NULL,"
        "numberOfWords INTEGER NOT NULL)");
    Sqlite::exec(
        "CREATE TABLE IF NOT EXISTS wordsFileContain ("
        "word TEXT UNIQUE,"
        "numberOfFileContains INTEGER NOT NULL)");
    Sqlite::exec(
        "CREATE TABLE IF NOT EXISTS wordsWeightByFile ("
        "fileId INTEGER NOT NULL,"
        "word TEXT NOT NULL,"
        "weight REAL NOT NULL,"
        "FOREIGN KEY(fileId) REFERENCES files(id))");
    // Create indexes
    Sqlite::exec("CREATE INDEX IF NOT EXISTS files_name ON files (name)");
    Sqlite::exec("CREATE INDEX IF NOT EXISTS files_path ON files (path)");
    Sqlite::exec(
        "CREATE INDEX IF NOT EXISTS files_mimetype ON files (mimetype)");
    Sqlite::exec(
        "CREATE INDEX IF NOT EXISTS files_numberOfWords ON files "
        "(numberOfWords)");
    Sqlite::exec(
        "CREATE INDEX IF NOT EXISTS wordsFileContain_word ON wordsFileContain "
        "(word)");
    Sqlite::exec(
        "CREATE INDEX IF NOT EXISTS wordsWeightByFile_word ON "
        "wordsWeightByFile "
        "(word)");
    Sqlite::exec(
        "CREATE INDEX IF NOT EXISTS wordsWeightByFile_fileId ON "
        "wordsWeightByFile "
        "(fileId)");

    sqlite3 *db = Sqlite::getDB();
    sqlite3_create_function(db, "levenshtein", 2,
                            SQLITE_UTF8 | SQLITE_DETERMINISTIC, NULL, distance,
                            NULL, NULL);
    TcpServer::getInstance().run();
    Sqlite::close();

    return 0;
}
