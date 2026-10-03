#ifndef NGRAM_H
#define NGRAM_H
#include <string>
#include "Vector.h"
using namespace std;
Vector<string> generateNGrams(const Vector<string>& tokens, int n) 
{
    Vector<string> ngrams;
    for (int i = 0; i + n <= tokens.getSize(); i++) 
    {
        string phrase = tokens[i];
        for (int j = 1; j < n; j++) phrase += " " + tokens[i + j];
        ngrams.push_back(phrase);
    }
    return ngrams;
}
unsigned long long fingerprint(const string& phrase) 
{
    unsigned long long h = 0;
    for (size_t i = 0; i < phrase.size(); i++)
        h = h * 131 + (unsigned char)phrase[i];
    return h;
}
#endif
