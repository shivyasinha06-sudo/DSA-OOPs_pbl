#include <iostream>
#include <fstream>
#include <sstream>
#include "Vector.h"
#include "HashTable.h"
#include "InvertedIndex.h"
#include "Trie.h"
#include "MaxHeap.h"
#include "Preprocessor.h"
#include "NGram.h"
#include "StringMatcher.h"
#include "Similarity.h"
#include "CorpusLoader.h"
#include "SQLiteDB.h"
using namespace std;
const int N = 4;               
const int TITLE_COL = 0;       
const int TEXT_COL = 1;
const int MAX_DOCS = 2000;     
bool readFile(const string& path, string& content) 
{
    ifstream file(path.c_str());
    if (!file) 
    {
        return false;
    }
    stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    return true;
}
void buildIndex(const Vector<Article>& corpus, InvertedIndex& index, Vector<int>& docNgramCount) 
{
    for (int i = 0; i < corpus.getSize(); i++) 
    {
        Vector<string> grams = generateNGrams(tokenize(corpus[i].text), N);
        HashTable seen;                         
        for (int g = 0; g < grams.getSize(); g++) 
        {
            ull fp = fingerprint(grams[g]);
            if (seen.insert(fp)) index.add(fp, i);
        }
        docNgramCount.push_back(seen.getCount());
    }
}
int main(int argc, char* argv[]) 
{
    string inputPath  = (argc >= 2) ? argv[1] : "data/input.txt";
    string corpusPath = (argc >= 3) ? argv[2] : "data/wiki.txt";   
    bool isCSV = corpusPath.size() >= 4 && corpusPath.substr(corpusPath.size() - 4) == ".csv";
    Vector<Article> corpus = isCSV ? loadCorpus(corpusPath, TITLE_COL, TEXT_COL, MAX_DOCS)
                                   : loadCorpusFromText(corpusPath, MAX_DOCS);
               SQLiteDB database;

if (!database.open("data/plagiarism.db")) {
    cerr << "Could not open database." << endl;
    return 1;
}

if (!database.createTable()) {
    cerr << "Could not create database table." << endl;
    return 1;
}                    
    if (corpus.getSize() == 0) 
    { 
        cout << "Could not load corpus: " << corpusPath << endl; 
        return 1; 
    }
    string rawInput;
    if (!readFile(inputPath, rawInput)) 
    { 
        cout << "Could not open input: " << inputPath << endl; 
        return 1; 
    }
    InvertedIndex index;
    Vector<int> docNgramCount;
    buildIndex(corpus, index, docNgramCount);
    string inputText = cleanText(rawInput);
    Vector<string> inputGrams = generateNGrams(tokenize(inputText), N);
    Vector<int> matchCount;                       
    for (int i = 0; i < corpus.getSize(); i++) 
    {
        matchCount.push_back(0);
    }
    HashTable seenInput;                          
    int covered = 0;                              
    for (int g = 0; g < inputGrams.getSize(); g++) 
    {
        ull fp = fingerprint(inputGrams[g]);
        if (!seenInput.insert(fp)) 
        {
            continue;
        }
        const Vector<int>* postings = index.find(fp);
        if (postings == NULL) 
        {
            continue;           
        }
        covered++;
        for (int k = 0; k < postings->getSize(); k++)
            matchCount[(*postings)[k]]++;
    }
    int inputUnique = seenInput.getCount();
    MaxHeap heap;
    for (int i = 0; i < corpus.getSize(); i++)
        if (matchCount[i] > 0)
            heap.push(Result(i, containment(matchCount[i], inputUnique)));
    cout << "IntelliPlag ran successfully. Articles indexed: " << corpus.getSize() << endl;
       // TEMP TEST - delete before committing
   for (int i = 0; i < 5; i++) cout << "Title: " << corpus[i].title << endl;
   if (!heap.isEmpty()) {
       Result top = heap.pop();
       cout << "Top match: " << corpus[top.docId].title << " (" << top.score << "% of input)" << endl;
   } else cout << "No match found" << endl;
    return 0;
}

