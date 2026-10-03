#ifndef STRINGMATCHER_H
#define STRINGMATCHER_H
#include <string>
#include "Vector.h"
using namespace std;
Vector<int> buildLPS(const string& pattern) 
{
    Vector<int> lps;
    lps.push_back(0);
    int len = 0;
    for (size_t i = 1; i < pattern.size(); i++) 
    {
        while (len > 0 && pattern[i] != pattern[len]) 
        {
            len = lps[len - 1];
        }
        if (pattern[i] == pattern[len]) 
        {
            len++;
        }
        lps.push_back(len);
    }
    return lps;
}
int kmpSearch(const string& text, const string& pattern) 
{
    if (pattern.empty()) 
    {
        return -1;
    }
    Vector<int> lps = buildLPS(pattern);
    int j = 0;  
    for (size_t i = 0; i < text.size(); i++) 
    {
        while (j > 0 && text[i] != pattern[j]) 
        {
            j = lps[j - 1];
        }
        if (text[i] == pattern[j]) 
        {
            j++;
        }
        if (j == (int)pattern.size()) 
        {
            return (int)(i - j + 1);
        }
    }
    return -1;
}
#endif
