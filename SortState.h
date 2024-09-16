#ifndef SORTSTATE_H
#define SORTSTATE_H

#include "CountingVector.h"
#include <mutex>

struct SortState
{
    CountingVector<int> numbers;
    std::mutex mtx;
    int comparisons;
    bool sortingComplete;
    bool running;
};

#endif // SORTSTATE_H
