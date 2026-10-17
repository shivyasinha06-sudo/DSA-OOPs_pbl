#include "CorpusLoader.h"
#include <sqlite3.h>
#include <algorithm>
#include <cctype>

// Implementation of SQLiteCorpusLoader added to keep behavior consistent
// with other loaders (CSV/Text). This implementation attempts to detect
// the most appropriate table and columns at runtime by inspecting the
// database schema (so it doesn't assume a fixed schema).

SQLiteCorpusLoader::SQLiteCorpusLoader(int maxD) : CorpusLoaderBase(maxD) {}

// Helper: lowercase a string
static std::string toLower(const std::string& s)
{
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c){ return std::tolower(c); });
    return out;
}

Vector<Article> SQLiteCorpusLoader::load(const std::string& path)
{
    Vector<Article> articles;
    sqlite3* db = nullptr;
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) return articles;

    // Find candidate tables (skip sqlite internal tables)
    const char* tblSql = "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';";
    sqlite3_stmt* tblStmt = nullptr;

    Vector<std::string> tables;
    if (sqlite3_prepare_v2(db, tblSql, -1, &tblStmt, nullptr) == SQLITE_OK)
    {
        while (sqlite3_step(tblStmt) == SQLITE_ROW)
        {
            const unsigned char* txt = sqlite3_column_text(tblStmt, 0);
            if (txt) tables.push_back(reinterpret_cast<const char*>(txt));
        }
        sqlite3_finalize(tblStmt);
    }

    // Search for a table and two columns we can use as (title, content).
    std::string chosenTable;
    std::string titleCol;
    std::string contentCol;

    auto isTextType = [](const std::string& t)->bool {
        std::string low = toLower(t);
        return (low.find("char") != std::string::npos) ||
               (low.find("text") != std::string::npos) ||
               (low.find("clob") != std::string::npos) ||
               (low.find("varchar") != std::string::npos) ||
               (low.empty()); // some SQLite schemas use no type
    };

    // Prefer columns named like title and content
    for (int ti = 0; ti < tables.getSize() && chosenTable.empty(); ti++)
    {
        const std::string& tname = tables[ti];
        std::string pragma = "PRAGMA table_info('" + tname + "');";
        sqlite3_stmt* colStmt = nullptr;
        Vector<std::pair<std::string,std::string>> cols; // (name,type)

        if (sqlite3_prepare_v2(db, pragma.c_str(), -1, &colStmt, nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(colStmt) == SQLITE_ROW)
            {
                const unsigned char* cname = sqlite3_column_text(colStmt, 1);
                const unsigned char* ctype = sqlite3_column_text(colStmt, 2);
                std::string name = cname ? reinterpret_cast<const char*>(cname) : "";
                std::string type = ctype ? reinterpret_cast<const char*>(ctype) : "";
                cols.push_back(std::make_pair(name, type));
            }
            sqlite3_finalize(colStmt);
        }

        // Look for good title/content candidates by name
        int titleIdx = -1, contentIdx = -1;
        for (int i = 0; i < cols.getSize(); i++)
        {
            std::string n = toLower(cols[i].first);
            if (titleIdx == -1 && (n == "title" || n == "name" || n == "headline" || n == "article_title"))
                titleIdx = i;
            if (contentIdx == -1 && (n == "content" || n == "text" || n == "body" || n == "article" || n == "content_text"))
                contentIdx = i;
        }

        // If no explicit names, pick any TEXT-like column as content and optional title
        if (contentIdx == -1)
        {
            for (int i = 0; i < cols.getSize(); i++)
            {
                if (isTextType(cols[i].second)) { contentIdx = i; break; }
            }
        }
        if (titleIdx == -1)
        {
            for (int i = 0; i < cols.getSize(); i++)
            {
                if (isTextType(cols[i].second) && i != contentIdx) { titleIdx = i; break; }
            }
        }

        if (contentIdx != -1)
        {
            chosenTable = tname;
            contentCol = cols[contentIdx].first;
            if (titleIdx != -1) titleCol = cols[titleIdx].first;
        }
    }

    if (chosenTable.empty())
    {
        sqlite3_close(db);
        return articles; // no suitable table found
    }

    // Build SQL depending on whether titleCol is present
    std::string sql;
    if (!titleCol.empty())
        sql = "SELECT \"" + titleCol + "\", \"" + contentCol + "\" FROM \"" + chosenTable + "\";";
    else
        sql = "SELECT \"" + contentCol + "\" FROM \"" + chosenTable + "\";";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW && articles.getSize() < maxDocs)
        {
            std::string title = "";
            std::string text = "";

            if (!titleCol.empty())
            {
                const unsigned char* t = sqlite3_column_text(stmt, 0);
                const unsigned char* c = sqlite3_column_text(stmt, 1);
                title = t ? reinterpret_cast<const char*>(t) : "";
                text  = c ? reinterpret_cast<const char*>(c) : "";
            }
            else
            {
                const unsigned char* c = sqlite3_column_text(stmt, 0);
                text  = c ? reinterpret_cast<const char*>(c) : "";
            }

            std::string cleaned = preprocessor.clean(text);
            if (cleaned.size() >= 50)
            {
                // If title is empty, generate a placeholder
                std::string finalTitle = title.empty() ? ("Article #" + std::to_string(articles.getSize() + 1)) : title;
                articles.push_back(Article(finalTitle, cleaned));
            }
        }
        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
    return articles;
}
