#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
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

using namespace std;

// OOP Concept: Master Engine Class encapsulating all DSA & OOP modules
class PlagiarismEngine 
{
private:
    int nGramSize;
    int maxDocs;
    Vector<Article> corpus;
    InvertedIndex index;
    Vector<int> docNgramCount;
    TextPreprocessor preprocessor;
    NGramGenerator ngramGen;
    KMPMatcher stringMatcher;

    bool readFile(const string& path, string& content) const 
    {
        ifstream file(path.c_str());

        if (!file)
            return false;

        stringstream buffer;
        buffer << file.rdbuf();

        content = buffer.str();

        return true;
    }

    void buildIndex() 
    {
        for (int i = 0; i < corpus.getSize(); i++) 
        {
            Vector<string> grams =
                ngramGen.generate(
                    preprocessor.tokenize(corpus[i].getText())
                );

            HashTable seen;

            for (int g = 0; g < grams.getSize(); g++) 
            {
                ull fp =
                    NGramGenerator::computeFingerprint(grams[g]);

                if (seen.insert(fp))
                    index.add(fp, i);
            }

            docNgramCount.push_back(seen.getCount());
        }
    }

public:

    PlagiarismEngine(int n = 4, int maxD = 2000)
        : nGramSize(n),
          maxDocs(maxD),
          ngramGen(n) {}

    // Load corpus from SQLite database
    bool initializeCorpus(const string& corpusPath) 
    {
        // Runtime Polymorphism:
        // Base class pointer points to SQLiteCorpusLoader object
        CorpusLoaderBase* loader =
            new SQLiteCorpusLoader(maxDocs);

        corpus = loader->load(corpusPath);

        delete loader;

        if (corpus.getSize() == 0)
            return false;

        buildIndex();

        return true;
    }

    void analyzeDocument(const string& inputPath) 
    {
        string rawInput;

        if (!readFile(inputPath, rawInput)) 
        {
            cout << "Error: Could not open input file: "
                 << inputPath << endl;

            return;
        }

        string inputText =
            preprocessor.clean(rawInput);

        Vector<string> inputGrams =
            ngramGen.generate(
                preprocessor.tokenize(inputText)
            );

        // DSA Integration: Prefix Trie for indexing input phrases
        Trie phraseTrie;

        Vector<int> matchCount;
        Vector<string> sampleMatchedPhrase;

        for (int i = 0; i < corpus.getSize(); i++) 
        {
            matchCount.push_back(0);
            sampleMatchedPhrase.push_back("");
        }

        HashTable seenInput;

        for (int g = 0; g < inputGrams.getSize(); g++) 
        {
            phraseTrie.insert(inputGrams[g]);

            ull fp =
                NGramGenerator::computeFingerprint(
                    inputGrams[g]
                );

            if (!seenInput.insert(fp))
                continue;

            const Vector<int>* postings =
                index.find(fp);

            if (postings == NULL)
                continue;

            for (int k = 0; k < postings->getSize(); k++) 
            {
                int docId = (*postings)[k];

                matchCount[docId]++;

                if (sampleMatchedPhrase[docId] == "")
                    sampleMatchedPhrase[docId] =
                        inputGrams[g];
            }
        }

        int inputUnique =
            seenInput.getCount();

        // Runtime Polymorphism:
        // Base class pointers for Similarity Metrics
        SimilarityMetric* containmentMetric =
            new ContainmentSimilarity();

        SimilarityMetric* jaccardMetric =
            new JaccardSimilarity();

        MaxHeap heap;

        for (int i = 0; i < corpus.getSize(); i++) 
        {
            if (matchCount[i] > 0) 
            {
                double score =
                    containmentMetric->compute(
                        matchCount[i],
                        inputUnique
                    );

                heap.push(
                    Result(
                        i,
                        score,
                        matchCount[i]
                    )
                );
            }
        }

        cout << "=========================================================="
             << endl;

        cout << "         PLAGICHECK: OOP & DSA DETECTION REPORT"
             << endl;

        cout << "=========================================================="
             << endl;

        cout << "Articles Indexed in Corpus : "
             << corpus.getSize()
             << endl;

        cout << "Unique "
             << nGramSize
             << "-Grams in Input   : "
             << inputUnique
             << endl;

        cout << "----------------------------------------------------------"
             << endl;

        if (heap.isEmpty()) 
        {
            cout << "Status: [CLEAN] No plagiarism matches found in corpus."
                 << endl;
        } 
        else 
        {
            int rank = 1;

            while (!heap.isEmpty() && rank <= 5) 
            {
                Result top = heap.pop();

                int id = top.getDocId();

                double contScore =
                    top.getScore();

                double jaccScore =
                    jaccardMetric->compute(
                        top.getMatchedGrams(),
                        inputUnique,
                        docNgramCount[id]
                    );

                string verdict =
                    (contScore >= 25.0)
                    ? "[PLAGIARISED]"
                    : "[SUSPICIOUS OVERLAP]";

                cout << "Rank #"
                     << rank++
                     << " -> "
                     << corpus[id].getTitle()
                     << " "
                     << verdict
                     << endl;

                cout << fixed << setprecision(2);

                cout << "  * "
                     << containmentMetric->getMetricName()
                     << " : "
                     << contScore
                     << "%"
                     << endl;

                cout << "  * "
                     << jaccardMetric->getMetricName()
                     << " : "
                     << jaccScore
                     << "%"
                     << endl;

                // DSA Integration:
                // KMP String Matcher + Trie verification
                string phrase =
                    sampleMatchedPhrase[id];

                if (phrase != "" &&
                    phraseTrie.search(phrase)) 
                {
                    int charPos =
                        stringMatcher.search(
                            inputText,
                            phrase
                        );

                    cout << "  * Matched Phrase       : \""
                         << phrase
                         << "\" (at char offset "
                         << charPos
                         << ")"
                         << endl;
                }

                cout << "----------------------------------------------------------"
                     << endl;
            }
        }

        delete containmentMetric;
        delete jaccardMetric;
    }
};

int main(int argc, char* argv[]) 
{
    string inputPath =
        (argc >= 2)
        ? argv[1]
        : "input.txt";

    // SQLite database is now the corpus source
    string corpusPath =
        (argc >= 3)
        ? argv[2]
        : "data/plagiarism.db";

    PlagiarismEngine engine(4, 2000);

    if (!engine.initializeCorpus(corpusPath)) 
    {
        cout << "Could not load corpus: "
             << corpusPath
             << endl;

        return 1;
    }

    engine.analyzeDocument(inputPath);

    return 0;
}