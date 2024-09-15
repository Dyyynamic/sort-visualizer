#ifndef SORTS_H
#define SORTS_H

#include "SortState.h"
#include "CountingVector.h"
#include <mutex>

void bubbleSort(
    CountingVector<int> &numbers,
    std::mutex &mtx,
    int &comparisons,
    int sortingDelay,
    bool &sortingComplete);

void selectionSort(
    CountingVector<int> &numbers,
    std::mutex &mtx,
    int &comparisons,
    int sortingDelay,
    bool &sortingComplete);

void insertionSort(
    CountingVector<int> &numbers,
    std::mutex &mtx,
    int &comparisons,
    int sortingDelay,
    bool &sortingComplete);

#endif // SORTS_H
