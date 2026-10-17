#ifndef STRINGMATCHER_H
#define STRINGMATCHER_H
#include <string>
#include "Vector.h"
using namespace std;

// OOP Concept: Abstract Interface
class PatternMatcher 
{
public:
    virtual ~PatternMatcher() {}
    virtual int search(const string& text, const string& pattern) const = 0;
};

// OOP Concept: Inheritance & Abstraction (Hiding LPS array construction)
class KMPMatcher : public PatternMatcher 
{
private:
    Vector<int> buildLPS(const string& pattern) const 
    {
        Vector<int> lps;
        lps.push_back(0);
        int len = 0;
        for (size_t i = 1; i < pattern.size(); i++) 
        {
            while (len > 0 && pattern[i] != pattern[len]) 
                len = lps[len - 1];
            if (pattern[i] == pattern[len]) len++;
            lps.push_back(len);
        }
        return lps;
    }

public:
    int search(const string& text, const string& pattern) const override 
    {
        if (pattern.empty()) return -1;
        Vector<int> lps = buildLPS(pattern);
        int j = 0;  
        for (size_t i = 0; i < text.size(); i++) 
        {
            while (j > 0 && text[i] != pattern[j]) 
                j = lps[j - 1];
            if (text[i] == pattern[j]) j++;
            if (j == (int)pattern.size()) 
                return (int)(i - j + 1);
        }
        return -1;
    }
};

inline int kmpSearch(const string& text, const string& pattern) {
    KMPMatcher matcher;
    return matcher.search(text, pattern);
}
#endif