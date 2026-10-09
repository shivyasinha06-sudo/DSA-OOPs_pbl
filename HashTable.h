#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "Vector.h"
typedef unsigned long long ull;
class HashTable 
{
private:
    struct Node 
    {            
        ull key;
        Node* next;
        Node(ull k) : key(k), next(NULL) {}
    };
    Node** buckets;          
    int bucketCount;
    int count;               
    int getIndex(ull key) const 
    { 
        return (int)(key % bucketCount); 
    }
    HashTable(const HashTable&);             
    HashTable& operator=(const HashTable&);
public:
    HashTable(int buckets_ = 10007) : bucketCount(buckets_), count(0) {
        buckets = new Node*[bucketCount];
        for (int i = 0; i < bucketCount; i++) buckets[i] = NULL;
    }
    ~HashTable() 
    {
        for (int i = 0; i < bucketCount; i++) {
            Node* cur = buckets[i];
            while (cur != NULL) {
                Node* temp = cur;
                cur = cur->next;
                delete temp;
            }
        }
        delete[] buckets;
    }
    bool contains(ull key) const {
        Node* cur = buckets[getIndex(key)];
        while (cur != NULL) 
        {
            if (cur->key == key) return true;
            cur = cur->next;
        }
        return false;
    }
    bool insert(ull key) {
        if (contains(key)) return false;
        int idx = getIndex(key);
        Node* node = new Node(key);
        node->next = buckets[idx];   
        buckets[idx] = node;
        count++;
        return true;
    }
    int getCount() const 
    { 
        return count; 
    }
    Vector<ull> getAllKeys() const 
    {
        Vector<ull> keys;
        for (int i = 0; i < bucketCount; i++) 
        {
            Node* cur = buckets[i];
            while (cur != NULL) 
            {
                keys.push_back(cur->key);
                cur = cur->next;
            }
        }
        return keys;
    }
};
#endif