#include "src/Sqlite.h"
#include "src/TcpServer.h"

int main(int, char**) {
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
    TcpServer::getInstance().run();
    Sqlite::close();

    return 0;
}
