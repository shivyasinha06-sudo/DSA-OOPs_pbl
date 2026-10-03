#ifndef CORPUSLOADER_H
#define CORPUSLOADER_H
#include <string>
#include <fstream>
#include <iostream>
#include "Vector.h"
#include "Preprocessor.h"
using namespace std;
struct Article 
{
    string title;
    string text;     
};
bool readCSVRow(istream& in, Vector<string>& row) 
{
    row = Vector<string>();
    string field = "";
    bool inQuotes = false;
    bool gotAny = false;
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
                    in.get(c); field += '"'; 
                }  
                else inQuotes = false;
            } 
            else 
            {
                field += c;
            }
        } 
        else 
        {
            if (c == '"') inQuotes = true;
            else if (c == ',') 
            { 
                row.push_back(field); field = ""; 
            }
            else if (c == '\n') 
            { 
                row.push_back(field); return true; 
            }
            else if (c != '\r') 
            { 
                field += c;
            }
        }
    }
    if (gotAny) 
    { 
        row.push_back(field); return true; 
    }
    return false;
}
Vector<Article> loadCorpus(const string& path, int titleCol, int textCol, int maxDocs) 
{
    Vector<Article> articles;
    ifstream file(path.c_str());
    if (!file) return articles;
    Vector<string> row;
    readCSVRow(file, row);                       
    while (articles.getSize() < maxDocs && readCSVRow(file, row)) 
    {
        if (textCol >= row.getSize() || titleCol >= row.getSize()) 
        {
            continue;
        }
        Article a;
        a.text = cleanText(row[textCol]);
        if (a.text.size() < 50) 
        {
            continue;
        }
        a.title = (titleCol >= 0) ? row[titleCol]
                                  : "Article #" + to_string(articles.getSize() + 1);
        articles.push_back(a);
    }
    return articles;
}
void stripCR(string& line) 
{
    if (!line.empty() && line[line.size() - 1] == '\r') line.erase(line.size() - 1);
}
bool isTitleLine(const string& line, bool prevBlank, bool nextBlank) 
{
    if (line.empty() || line.size() > 60) return false;
    if (line[line.size() - 1] == '.') return false;
    return prevBlank && nextBlank;
}
Vector<Article> loadCorpusFromText(const string& path, int maxDocs) 
{
    Vector<Article> articles;
    ifstream file(path.c_str());
    if (!file) 
    {
        return articles;
    }
    string line, nextLine;
    string title = "", body = "";
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
            string text = cleanText(body);
            if (text.size() >= 50) 
            {
                Article a;
                a.title = title;
                a.text = text;
                articles.push_back(a);
                if (articles.getSize() >= maxDocs) 
                {
                    return articles;
                }
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
    string text = cleanText(body);
    if (text.size() >= 50 && articles.getSize() < maxDocs) 
    {
        Article a;
        a.title = title;
        a.text = text;
        articles.push_back(a);
    }
    return articles;
}
#endif
