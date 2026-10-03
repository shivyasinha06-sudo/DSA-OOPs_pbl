#ifndef SIMILARITY_H
#define SIMILARITY_H
double jaccardSimilarity(int common, int sizeA, int sizeB) 
{
    int unionSize = sizeA + sizeB - common;
    if (unionSize == 0) return 0.0;
    return 100.0 * common / unionSize;
}
double containment(int common, int sizeInput) 
{
    if (sizeInput == 0) return 0.0;
    return 100.0 * common / sizeInput;
}
#endif
