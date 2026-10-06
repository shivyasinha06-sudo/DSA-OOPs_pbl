#ifndef SQLITE_DB_H
#define SQLITE_DB_H

#include <string>
#include <vector>
#include <sqlite3.h>

using namespace std;

struct DBArticle {
    int id;
    string title;
    string content;
};

class SQLiteDB {
private:
    sqlite3* db;

public:
    SQLiteDB() : db(nullptr) {}

    ~SQLiteDB() {
        close();
    }

    bool open(const string& path) {
        return sqlite3_open(path.c_str(), &db) == SQLITE_OK;
    }

    void close() {
        if (db != nullptr) {
            sqlite3_close(db);
            db = nullptr;
        }
    }

    bool createTable() {
        const char* sql =
            "CREATE TABLE IF NOT EXISTS articles ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "title TEXT,"
            "content TEXT"
            ");";

        char* errorMessage = nullptr;

        int result = sqlite3_exec(
            db,
            sql,
            nullptr,
            nullptr,
            &errorMessage
        );

        if (result != SQLITE_OK) {
            sqlite3_free(errorMessage);
            return false;
        }

        return true;
    }

    bool insertArticle(const string& title, const string& content) {
        const char* sql =
            "INSERT INTO articles (title, content) VALUES (?, ?);";

        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK)
            return false;

        sqlite3_bind_text(
            statement,
            1,
            title.c_str(),
            -1,
            SQLITE_TRANSIENT
        );

        sqlite3_bind_text(
            statement,
            2,
            content.c_str(),
            -1,
            SQLITE_TRANSIENT
        );

        int result = sqlite3_step(statement);

        sqlite3_finalize(statement);

        return result == SQLITE_DONE;
    }

    int getArticleCount() {
        const char* sql = "SELECT COUNT(*) FROM articles;";
        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK)
            return 0;

        int count = 0;

        if (sqlite3_step(statement) == SQLITE_ROW)
            count = sqlite3_column_int(statement, 0);

        sqlite3_finalize(statement);

        return count;
    }
};

#endif