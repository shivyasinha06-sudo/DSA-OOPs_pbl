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

// Escape a string for safe JSON embedding
static string jsonEscape(const string& s)
{
    string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); i++)
    {
        unsigned char c = s[i];
        if      (c == '"')  out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else if (c < 0x20)  { /* skip other control chars */ }
        else                out += (char)c;
    }
    return out;
}

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
        if (!file) return false;
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
                ull fp = NGramGenerator::computeFingerprint(grams[g]);
                if (seen.insert(fp))
                    index.add(fp, i);
            }

            docNgramCount.push_back(seen.getCount());
        }
    }

public:
    PlagiarismEngine(int n = 4, int maxD = 2000)
        : nGramSize(n), maxDocs(maxD), ngramGen(n) {}

    bool initializeCorpus(const string& corpusPath)
    {
        // Runtime Polymorphism: base pointer -> SQLiteCorpusLoader
        CorpusLoaderBase* loader = new SQLiteCorpusLoader(maxDocs);
        corpus = loader->load(corpusPath);
        delete loader;

        if (corpus.getSize() == 0) return false;

        buildIndex();
        return true;
    }

    // ----------------------------------------------------------------
    // analyzeDocument: runs the real DSA analysis.
    // jsonMode=true  -> prints a single JSON object to stdout.
    // jsonMode=false -> prints the original human-readable report.
    // ----------------------------------------------------------------
    void analyzeDocument(const string& inputPath, bool jsonMode = false)
    {
        string rawInput;
        if (!readFile(inputPath, rawInput))
        {
            if (jsonMode)
                cout << "{\"error\":\"Could not open input file: "
                     << jsonEscape(inputPath) << "\"}" << endl;
            else
                cout << "Error: Could not open input file: "
                     << inputPath << endl;
            return;
        }

        string inputText       = preprocessor.clean(rawInput);
        Vector<string> inputGrams =
            ngramGen.generate(preprocessor.tokenize(inputText));

        // DSA Integration: Prefix Trie for indexing input phrases
        Trie phraseTrie;

        Vector<int>    matchCount;
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

            ull fp = NGramGenerator::computeFingerprint(inputGrams[g]);
            if (!seenInput.insert(fp)) continue;

            const Vector<int>* postings = index.find(fp);
            if (postings == NULL) continue;

            for (int k = 0; k < postings->getSize(); k++)
            {
                int docId = (*postings)[k];
                matchCount[docId]++;
                if (sampleMatchedPhrase[docId] == "")
                    sampleMatchedPhrase[docId] = inputGrams[g];
            }
        }

        int inputUnique = seenInput.getCount();

        // Runtime Polymorphism: Similarity Metrics
        SimilarityMetric* containmentMetric = new ContainmentSimilarity();
        SimilarityMetric* jaccardMetric     = new JaccardSimilarity();

        MaxHeap heap;

        for (int i = 0; i < corpus.getSize(); i++)
        {
            if (matchCount[i] > 0)
            {
                double score = containmentMetric->compute(
                    matchCount[i], inputUnique);
                heap.push(Result(i, score, matchCount[i]));
            }
        }

        // ---- JSON output path ----
        if (jsonMode)
        {
            cout << "{";
            cout << "\"articlesIndexed\":" << corpus.getSize() << ",";
            cout << "\"uniqueNGrams\":"    << inputUnique       << ",";
            cout << "\"nGramSize\":"       << nGramSize         << ",";

            if (heap.isEmpty())
            {
                cout << "\"status\":\"CLEAN\","
                     << "\"matches\":[]";
            }
            else
            {
                cout << "\"status\":\"MATCHES_FOUND\","
                     << "\"matches\":[";

                int rank = 1;
                bool first = true;

                while (!heap.isEmpty() && rank <= 5)
                {
                    Result top = heap.pop();
                    int    id  = top.getDocId();

                    double contScore  = top.getScore();
                    double jaccScore  = jaccardMetric->compute(
                        top.getMatchedGrams(), inputUnique, docNgramCount[id]);

                    string verdict =
                        (contScore >= 25.0) ? "PLAGIARISED" : "SUSPICIOUS_OVERLAP";

                    string phrase    = sampleMatchedPhrase[id];
                    int    charPos   = -1;

                    if (phrase != "" && phraseTrie.search(phrase))
                        charPos = stringMatcher.search(inputText, phrase);

                    if (!first) cout << ",";
                    first = false;

                    cout << fixed << setprecision(2);
                    cout << "{"
                         << "\"rank\":"        << rank                         << ","
                         << "\"title\":\""     << jsonEscape(corpus[id].getTitle()) << "\","
                         << "\"verdict\":\""   << verdict                      << "\","
                         << "\"containment\":" << contScore                    << ","
                         << "\"jaccard\":"     << jaccScore                    << ","
                         << "\"matchedPhrase\":\"" << jsonEscape(phrase)       << "\","
                         << "\"charOffset\":"  << charPos
                         << "}";
                    rank++;
                }

                cout << "]";
            }

            cout << "}" << endl;
        }
        // ---- Human-readable output path (original, unchanged) ----
        else
        {
            cout << "=========================================================="
                 << endl;
            cout << "         PLAGICHECK: OOP & DSA DETECTION REPORT" << endl;
            cout << "=========================================================="
                 << endl;
            cout << "Articles Indexed in Corpus : " << corpus.getSize() << endl;
            cout << "Unique " << nGramSize
                 << "-Grams in Input   : " << inputUnique << endl;
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
                    int    id  = top.getDocId();

                    double contScore = top.getScore();
                    double jaccScore = jaccardMetric->compute(
                        top.getMatchedGrams(), inputUnique, docNgramCount[id]);

                    string verdict =
                        (contScore >= 25.0) ? "[PLAGIARISED]" : "[SUSPICIOUS OVERLAP]";

                    cout << "Rank #" << rank++ << " -> "
                         << corpus[id].getTitle() << " " << verdict << endl;

                    cout << fixed << setprecision(2);
                    cout << "  * " << containmentMetric->getMetricName()
                         << " : " << contScore << "%" << endl;
                    cout << "  * " << jaccardMetric->getMetricName()
                         << " : " << jaccScore << "%" << endl;

                    // DSA Integration: KMP String Matcher + Trie verification
                    string phrase = sampleMatchedPhrase[id];
                    if (phrase != "" && phraseTrie.search(phrase))
                    {
                        int charPos = stringMatcher.search(inputText, phrase);
                        cout << "  * Matched Phrase       : \""
                             << phrase << "\" (at char offset " << charPos << ")" << endl;
                    }

                    cout << "----------------------------------------------------------"
                         << endl;
                }
            }
        }

        delete containmentMetric;
        delete jaccardMetric;
    }
};

int main(int argc, char* argv[])
{
    // Parse arguments:
    //   main.exe [--json] <inputFile> [<dbPath>]
    bool   jsonMode   = false;
    string inputPath  = "input.txt";
    string corpusPath = "data/plagiarism.db";

    for (int i = 1; i < argc; i++)
    {
        string arg = argv[i];
        if (arg == "--json")
            jsonMode = true;
        else if (inputPath == "input.txt")
            inputPath = arg;
        else
            corpusPath = arg;
    }

    PlagiarismEngine engine(4, 2000);

    if (!engine.initializeCorpus(corpusPath))
    {
        if (jsonMode)
            cout << "{\"error\":\"Could not load corpus: "
                 << jsonEscape(corpusPath) << "\"}" << endl;
        else
            cout << "Could not load corpus: " << corpusPath << endl;
        return 1;
    }

    engine.analyzeDocument(inputPath, jsonMode);
    return 0;
}
