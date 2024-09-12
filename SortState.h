#ifndef SORTSTATE_H
#define SORTSTATE_H

#include "CountingVector.h"

struct SortState
{
    CountingVector<int> numbers;
    int cursorPos;
    bool sorted;
    bool verified;
    int comparisons;
    int sortedIndex;
    int checkingPos;
    int currentMinIndex;
};

#endif // SORTSTATE_H
