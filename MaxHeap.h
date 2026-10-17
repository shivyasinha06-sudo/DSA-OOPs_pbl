#ifndef MAXHEAP_H
#define MAXHEAP_H
#include "Vector.h"

// OOP Concept: Encapsulation & Operator Overloading (Compile-Time Polymorphism)
class Result 
{
public:
    int docId;
    double score;
    int matchedGrams;

    Result() : docId(-1), score(0.0), matchedGrams(0) {}
    Result(int d, double s, int m = 0) : docId(d), score(s), matchedGrams(m) {}

    int getDocId() const { return docId; }
    double getScore() const { return score; }
    int getMatchedGrams() const { return matchedGrams; }

    // Operator Overloading
    bool operator>(const Result& other) const { return score > other.score; }
    bool operator<(const Result& other) const { return score < other.score; }
    bool operator<=(const Result& other) const { return score <= other.score; }
};

class MaxHeap {
private:
    Vector<Result> data;     
    void swapItems(int a, int b) 
    {
        Result temp = data[a];
        data[a] = data[b];
        data[b] = temp;
    }
    void heapifyUp(int i) 
    {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (data[i] <= data[parent]) break; // Uses overloaded <= operator
            swapItems(i, parent);
            i = parent;
        }
    }
    void heapifyDown(int i) 
    {
        int n = data.getSize();
        while (true) 
        {
            int left = 2 * i + 1, right = 2 * i + 2, largest = i;
            if (left < n && data[left] > data[largest]) largest = left;   // Uses overloaded >
            if (right < n && data[right] > data[largest]) largest = right;
            if (largest == i) break;
            swapItems(i, largest);
            i = largest;
        }
    }
public:
    bool isEmpty() const { return data.getSize() == 0; }
    int getSize() const { return data.getSize(); }
    void push(const Result& r) 
    {
        data.push_back(r);
        heapifyUp(data.getSize() - 1);
    }
    Result pop() 
    {
        Result top = data[0];
        int last = data.getSize() - 1;
        data[0] = data[last];
        Vector<Result> smaller;
        for (int i = 0; i < last; i++) smaller.push_back(data[i]);
        data = smaller;
        if (!isEmpty()) heapifyDown(0);
        return top;
    }
};
#endif