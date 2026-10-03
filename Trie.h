#ifndef TRIE_H
#define TRIE_H
#include <string>
using namespace std;
class Trie 
{
private:
    struct TrieNode 
    {
        TrieNode* children[37];   
        bool isEndOfPhrase;
        TrieNode() : isEndOfPhrase(false) 
        {
            for (int i = 0; i < 37; i++) children[i] = NULL;
        }
    };
    TrieNode* root;
    int charToIndex(char c) const 
    {
        if (c >= 'a' && c <= 'z') return c - 'a';
        if (c >= '0' && c <= '9') return 26 + (c - '0');
        return 36;                
    }
    void destroy(TrieNode* node) 
    {
        if (node == NULL) 
        {
            return;
        }
        for (int i = 0; i < 37; i++) 
        {
            destroy(node->children[i]);
        }
        delete node;
    }
    Trie(const Trie&);
    Trie& operator=(const Trie&);
public:
    Trie() : root(new TrieNode()) {}
    ~Trie() 
    { 
        destroy(root); 
    }

    void insert(const string& phrase) 
    {
        TrieNode* cur = root;
        for (size_t i = 0; i < phrase.size(); i++) 
        {
            int idx = charToIndex(phrase[i]);
            if (cur->children[idx] == NULL) cur->children[idx] = new TrieNode();
            cur = cur->children[idx];
        }
        cur->isEndOfPhrase = true;
    }
    bool search(const string& phrase) const 
    {
        TrieNode* cur = root;
        for (size_t i = 0; i < phrase.size(); i++) 
        {
            cur = cur->children[charToIndex(phrase[i])];
            if (cur == NULL) return false;
        }
        return cur->isEndOfPhrase;
    }
    bool startsWith(const string& prefix) const 
    {
        TrieNode* cur = root;
        for (size_t i = 0; i < prefix.size(); i++) 
        {
            cur = cur->children[charToIndex(prefix[i])];
            if (cur == NULL) return false;
        }
        return true;
    }
};
#endif
