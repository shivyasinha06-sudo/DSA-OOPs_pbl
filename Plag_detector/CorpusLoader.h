#ifndef CORPUSLOADER_H
#define CORPUSLOADER_H
#include <string>
#include <fstream>
#include <iostream>
#include "Vector.h"
#include "Preprocessor.h"
using namespace std;
struct Article {
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
    if (gotAny) { row.push_back(field); return true; }
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
#endif
