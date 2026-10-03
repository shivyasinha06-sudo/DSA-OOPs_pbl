#ifndef MAXHEAP_H
#define MAXHEAP_H
#include "Vector.h"
struct Result 
{
    int docId;
    double score;
    Result() : docId(-1), score(0) {}
    Result(int d, double s) : docId(d), score(s) {}
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
            if (data[i].score <= data[parent].score) 
            {
                break;
            }
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
            if (left < n && data[left].score > data[largest].score) largest = left;
            if (right < n && data[right].score > data[largest].score) largest = right;
            if (largest == i) break;
            swapItems(i, largest);
            i = largest;
        }
    }
public:
    bool isEmpty() const 
    { 
        return data.getSize() == 0; 
    }
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
        for (int i = 0; i < last; i++) 
        {
            smaller.push_back(data[i]);
        }
        data = smaller;
        if (!isEmpty()) heapifyDown(0);
        return top;
    }
};
#endif
