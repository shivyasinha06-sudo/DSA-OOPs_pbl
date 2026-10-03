#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include <string>
#include <cctype>
#include "Vector.h"
using namespace std;
string toLowerCase(const string& text) 
{
    string result = text;
    for (size_t i = 0; i < result.size(); i++)
        result[i] = tolower((unsigned char)result[i]);
    return result;
}
string removePunctuation(const string& text) 
{
    string result = "";
    for (size_t i = 0; i < text.size(); i++) 
    {
        unsigned char c = text[i];
        if (isalnum(c)) 
        {
            result += (char)c;
        }
        else 
        {
            result += ' ';
        }
    }
    return result;
}
Vector<string> tokenize(const string& text) 
{
    Vector<string> tokens;
    string word = "";
    for (size_t i = 0; i < text.size(); i++) 
    {
        if (text[i] == ' ') 
        {
            if (word != "") 
            { 
                tokens.push_back(word); word = ""; 
            }
        } 
        else 
        {
            word += text[i];
        }
    }
    if (word != "") 
    {
        tokens.push_back(word);
    }
    return tokens;
}
string cleanText(const string& raw) 
{
    Vector<string> words = tokenize(removePunctuation(toLowerCase(raw)));
    string result = "";
    for (int i = 0; i < words.getSize(); i++) 
    {
        if (i > 0) result += " ";
        result += words[i];
    }
    return result;
}
#endif
