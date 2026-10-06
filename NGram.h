#ifndef NGRAM_H
#define NGRAM_H
#include <string>
#include "Vector.h"
using namespace std;

// OOP Concept: Encapsulation & Data Hiding
class NGramGenerator 
{
private:
    int n; // Encapsulated N-gram window size

public:
    NGramGenerator(int windowSize = 4) : n(windowSize) {}

    int getN() const { return n; }
    void setN(int windowSize) { if (windowSize > 0) n = windowSize; }

    Vector<string> generate(const Vector<string>& tokens) const 
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

    static unsigned long long computeFingerprint(const string& phrase) 
    {
        unsigned long long h = 0;
        for (size_t i = 0; i < phrase.size(); i++)
            h = h * 131 + (unsigned char)phrase[i];
        return h;
    }
};

// Wrapper functions for compatibility
inline Vector<string> generateNGrams(const Vector<string>& tokens, int n) {
    NGramGenerator gen(n);
    return gen.generate(tokens);
}
inline unsigned long long fingerprint(const string& phrase) {
    return NGramGenerator::computeFingerprint(phrase);
}
#endif