#ifndef SIMILARITY_H
#define SIMILARITY_H
#include <string>
using namespace std;

// OOP Concept: Abstraction (Abstract Base Class with Pure Virtual Functions)
class SimilarityMetric 
{
public:
    virtual ~SimilarityMetric() {}
    virtual double compute(int common, int sizeA, int sizeB = 0) const = 0;
    virtual string getMetricName() const = 0;
};

// OOP Concept: Inheritance & Polymorphism (Derived Class 1)
class JaccardSimilarity : public SimilarityMetric 
{
public:
    double compute(int common, int sizeA, int sizeB = 0) const override 
    {
        int unionSize = sizeA + sizeB - common;
        if (unionSize <= 0) return 0.0;
        return 100.0 * common / unionSize;
    }
    string getMetricName() const override { return "Jaccard Similarity"; }
};

// OOP Concept: Inheritance & Polymorphism (Derived Class 2)
class ContainmentSimilarity : public SimilarityMetric 
{
public:
    double compute(int common, int sizeInput, int sizeDoc = 0) const override 
    {
        if (sizeInput <= 0) return 0.0;
        return 100.0 * common / sizeInput;
    }
    string getMetricName() const override { return "Containment Score"; }
};

// Wrapper functions for compatibility
inline double jaccardSimilarity(int common, int sizeA, int sizeB) {
    JaccardSimilarity js;
    return js.compute(common, sizeA, sizeB);
}
inline double containment(int common, int sizeInput) {
    ContainmentSimilarity cs;
    return cs.compute(common, sizeInput);
}
#endif