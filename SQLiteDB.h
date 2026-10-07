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

    // Original single-article insert function
    bool insertArticle(
        const string& title,
        const string& content
    ) {
        const char* sql =
            "INSERT INTO articles (title, content) VALUES (?, ?);";

        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare_v2(
                db,
                sql,
                -1,
                &statement,
                nullptr
            ) != SQLITE_OK)
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

    bool clearArticles() {
        const char* sql = "DELETE FROM articles;";

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

    // ------------------------------------------
    // BULK IMPORT SUPPORT
    // ------------------------------------------

    bool beginTransaction() {
        return sqlite3_exec(
            db,
            "BEGIN TRANSACTION;",
            nullptr,
            nullptr,
            nullptr
        ) == SQLITE_OK;
    }

    bool commitTransaction() {
        return sqlite3_exec(
            db,
            "COMMIT;",
            nullptr,
            nullptr,
            nullptr
        ) == SQLITE_OK;
    }

    bool rollbackTransaction() {
        return sqlite3_exec(
            db,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        ) == SQLITE_OK;
    }

    // Prepare INSERT statement once.
    sqlite3_stmt* prepareInsertStatement() {
        const char* sql =
            "INSERT INTO articles (title, content) VALUES (?, ?);";

        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare_v2(
                db,
                sql,
                -1,
                &statement,
                nullptr
            ) != SQLITE_OK)
        {
            return nullptr;
        }

        return statement;
    }

    // Reuse the same prepared INSERT statement.
    bool insertArticlePrepared(
        sqlite3_stmt* statement,
        const string& title,
        const string& content
    ) {
        if (statement == nullptr)
            return false;

        sqlite3_reset(statement);
        sqlite3_clear_bindings(statement);

        if (sqlite3_bind_text(
                statement,
                1,
                title.c_str(),
                -1,
                SQLITE_TRANSIENT
            ) != SQLITE_OK)
        {
            return false;
        }

        if (sqlite3_bind_text(
                statement,
                2,
                content.c_str(),
                -1,
                SQLITE_TRANSIENT
            ) != SQLITE_OK)
        {
            return false;
        }

        int result = sqlite3_step(statement);

        if (result != SQLITE_DONE)
            return false;

        return true;
    }

    void finalizeStatement(sqlite3_stmt* statement) {
        if (statement != nullptr)
            sqlite3_finalize(statement);
    }

    // ------------------------------------------
    // READ ARTICLES
    // ------------------------------------------

    int getArticleCount() {
        const char* sql =
            "SELECT COUNT(*) FROM articles;";

        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare_v2(
                db,
                sql,
                -1,
                &statement,
                nullptr
            ) != SQLITE_OK)
            return 0;

        int count = 0;

        if (sqlite3_step(statement) == SQLITE_ROW)
            count = sqlite3_column_int(statement, 0);

        sqlite3_finalize(statement);

        return count;
    }

    vector<DBArticle> getArticles() {
        vector<DBArticle> articles;

        const char* sql =
            "SELECT id, title, content FROM articles;";

        sqlite3_stmt* statement = nullptr;

        if (sqlite3_prepare_v2(
                db,
                sql,
                -1,
                &statement,
                nullptr
            ) != SQLITE_OK)
            return articles;

        while (sqlite3_step(statement) == SQLITE_ROW) {

            DBArticle article;

            article.id =
                sqlite3_column_int(statement, 0);

            const unsigned char* title =
                sqlite3_column_text(statement, 1);

            const unsigned char* content =
                sqlite3_column_text(statement, 2);

            article.title =
                title
                ? reinterpret_cast<const char*>(title)
                : "";

            article.content =
                content
                ? reinterpret_cast<const char*>(content)
                : "";

            articles.push_back(article);
        }

        sqlite3_finalize(statement);

        return articles;
    }
};

#endif