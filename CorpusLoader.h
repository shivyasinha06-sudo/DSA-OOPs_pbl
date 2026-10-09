#ifndef CORPUSLOADER_H
#define CORPUSLOADER_H

#include <string>
#include <fstream>
#include <iostream>
#include "Vector.h"
#include "Preprocessor.h"
#include "SQLiteDB.h"

using namespace std;

// OOP Concept: Class with Constructors & Getters
class Article
{
public:
    string title;
    string text;

    Article() : title(""), text("") {}
    Article(const string& t, const string& txt) : title(t), text(txt) {}

    string getTitle() const { return title; }
    string getText() const { return text; }
};

// OOP Concept: Abstract Base Class (Inheritance & Polymorphism)
class CorpusLoaderBase
{
protected:
    int maxDocs;
    TextPreprocessor preprocessor;

public:
    CorpusLoaderBase(int maxD = 2000) : maxDocs(maxD) {}
    virtual ~CorpusLoaderBase() {}

    virtual Vector<Article> load(const string& path) = 0;
};

// Derived Class: SQLite Database Corpus Loader
class SQLiteCorpusLoader : public CorpusLoaderBase
{
public:
    SQLiteCorpusLoader(int maxD = 2000)
        : CorpusLoaderBase(maxD) {}

    Vector<Article> load(const string& path) override
    {
        Vector<Article> articles;

        SQLiteDB database;

        if (!database.open(path))
            return articles;

        if (!database.createTable())
        {
            database.close();
            return articles;
        }

        vector<DBArticle> dbArticles = database.getArticles();

        for (size_t i = 0; i < dbArticles.size(); i++)
        {
            if (articles.getSize() >= maxDocs)
                break;

            string cleaned = preprocessor.clean(dbArticles[i].content);

            if (cleaned.size() < 50)
                continue;

            articles.push_back(
                Article(dbArticles[i].title, cleaned)
            );
        }

        database.close();

        return articles;
    }
};

// Derived Class 1: CSV Corpus Loader
// Kept temporarily so the old implementation still exists.
// We will remove it after the SQLite version is working.
class CSVCorpusLoader : public CorpusLoaderBase
{
private:
    int titleCol;
    int textCol;

    bool readCSVRow(istream& in, Vector<string>& row) const
    {
        row = Vector<string>();
        string field = "";
        bool inQuotes = false, gotAny = false;
        char c;

        while (in.get(c))
        {
            gotAny = true;

            if (inQuotes)
            {
                if (c == '"')
                {
                    if (in.peek() == '"')
                    {
                        in.get(c);
                        field += '"';
                    }
                    else
                    {
                        inQuotes = false;
                    }
                }
                else
                {
                    field += c;
                }
            }
            else
            {
                if (c == '"')
                    inQuotes = true;
                else if (c == ',')
                {
                    row.push_back(field);
                    field = "";
                }
                else if (c == '\n')
                {
                    row.push_back(field);
                    return true;
                }
                else if (c != '\r')
                {
                    field += c;
                }
            }
        }

        if (gotAny)
        {
            row.push_back(field);
            return true;
        }

        return false;
    }

public:
    CSVCorpusLoader(int tCol = 0, int txtCol = 1, int maxD = 2000)
        : CorpusLoaderBase(maxD), titleCol(tCol), textCol(txtCol) {}

    Vector<Article> load(const string& path) override
    {
        Vector<Article> articles;

        ifstream file(path.c_str());

        if (!file)
            return articles;

        Vector<string> row;

        readCSVRow(file, row); // skip header

        while (articles.getSize() < maxDocs && readCSVRow(file, row))
        {
            if (textCol >= row.getSize() || titleCol >= row.getSize())
                continue;

            string cleaned = preprocessor.clean(row[textCol]);

            if (cleaned.size() < 50)
                continue;

            string t = (titleCol >= 0)
                ? row[titleCol]
                : "Article #" + to_string(articles.getSize() + 1);

            articles.push_back(Article(t, cleaned));
        }

        return articles;
    }
};

// Derived Class 2: Plain Text Corpus Loader
class TextCorpusLoader : public CorpusLoaderBase
{
private:
    void stripCR(string& line) const
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
    }

    bool isTitleLine(
        const string& line,
        bool prevBlank,
        bool nextBlank
    ) const
    {
        if (line.empty() || line.size() > 60)
            return false;

        if (line[line.size() - 1] == '.')
            return false;

        return prevBlank && nextBlank;
    }

public:
    TextCorpusLoader(int maxD = 2000)
        : CorpusLoaderBase(maxD) {}

    Vector<Article> load(const string& path) override
    {
        Vector<Article> articles;

        ifstream file(path.c_str());

        if (!file)
            return articles;

        string line, nextLine, title = "", body = "";

        bool prevBlank = true;

        bool haveLine = (bool)getline(file, line);

        while (haveLine)
        {
            stripCR(line);

            bool haveNext = (bool)getline(file, nextLine);

            stripCR(nextLine);

            bool nextBlank = haveNext && nextLine.empty();

            if (isTitleLine(line, prevBlank, nextBlank))
            {
                string text = preprocessor.clean(body);

                if (text.size() >= 50)
                {
                    articles.push_back(
                        Article(title, text)
                    );

                    if (articles.getSize() >= maxDocs)
                        return articles;
                }

                title = line;
                body = "";
            }
            else
            {
                body += line + " ";
            }

            prevBlank = line.empty();

            line = nextLine;
            haveLine = haveNext;
        }

        string text = preprocessor.clean(body);

        if (text.size() >= 50 &&
            articles.getSize() < maxDocs)
        {
            articles.push_back(
                Article(title, text)
            );
        }

        return articles;
    }
};

#endif