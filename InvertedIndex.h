#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H
#include "Vector.h"
#include "HashTable.h"   
class InvertedIndex 
{
private:
    struct Node 
    {
        ull key;
        Vector<int> docIds;     
        Node* next;
        Node(ull k) : key(k), next(NULL) {}
    };
    Node** buckets;
    int bucketCount;
    Node* findNode(ull key) const 
    {
        Node* cur = buckets[key % bucketCount];
        while (cur != NULL) 
        {
            if (cur->key == key) return cur;
            cur = cur->next;
        }
        return NULL;
    }
    InvertedIndex(const InvertedIndex&);
    InvertedIndex& operator=(const InvertedIndex&);
public:
    InvertedIndex(int size = 200003) : bucketCount(size) 
    {
        buckets = new Node*[bucketCount];
        for (int i = 0; i < bucketCount; i++) buckets[i] = NULL;
    }
    ~InvertedIndex() {
        for (int i = 0; i < bucketCount; i++) 
        {
            Node* cur = buckets[i];
            while (cur != NULL) 
            { 
                Node* t = cur; cur = cur->next; delete t; 
            }
        }
        delete[] buckets;
    }
    void add(ull key, int docId) 
    {
        Node* node = findNode(key);
        if (node == NULL) 
        {
            node = new Node(key);
            int idx = (int)(key % bucketCount);
            node->next = buckets[idx];
            buckets[idx] = node;
        }
        int n = node->docIds.getSize();
        if (n > 0 && node->docIds[n - 1] == docId) 
        {
            return;
        }
        node->docIds.push_back(docId);
    }
    const Vector<int>* find(ull key) const 
    {
        Node* node = findNode(key);
        return node ? &node->docIds : NULL;
    }
};
#endif
