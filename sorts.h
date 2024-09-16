#ifndef SORTS_H
#define SORTS_H

#include <atomic>
#include "SortState.h"
#include "CountingVector.h"

void bubbleSort(SortState &state, int sortingDelay, std::atomic<bool> &running);
void selectionSort(SortState &state, int sortingDelay, std::atomic<bool> &running);
void insertionSort(SortState &state, int sortingDelay, std::atomic<bool> &running);

#endif // SORTS_H
