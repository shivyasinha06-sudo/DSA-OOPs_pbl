#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include <string>
#include <cctype>
#include "Vector.h"
using namespace std;

// OOP Concept: Encapsulation & Abstraction
class TextPreprocessor 
{
private:
    string toLowerCase(const string& text) const 
    {
        string result = text;
        for (size_t i = 0; i < result.size(); i++)
            result[i] = tolower((unsigned char)result[i]);
        return result;
    }

    string removePunctuation(const string& text) const 
    {
        string result = "";
        for (size_t i = 0; i < text.size(); i++) 
        {
            unsigned char c = text[i];
            if (isalnum(c)) result += (char)c;
            else result += ' ';
        }
        return result;
    }

public:
    TextPreprocessor() {}

    Vector<string> tokenize(const string& text) const 
    {
        Vector<string> tokens;
        string word = "";
        for (size_t i = 0; i < text.size(); i++) 
        {
            if (text[i] == ' ') 
            {
                if (word != "") 
                { 
                    tokens.push_back(word); 
                    word = ""; 
                }
            } 
            else word += text[i];
        }
        if (word != "") tokens.push_back(word);
        return tokens;
    }

    string clean(const string& raw) const 
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
};

// Wrapper functions for compatibility
inline Vector<string> tokenize(const string& text) {
    TextPreprocessor tp;
    return tp.tokenize(text);
}
inline string cleanText(const string& raw) {
    TextPreprocessor tp;
    return tp.clean(raw);
}
#endif