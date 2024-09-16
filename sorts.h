#ifndef SORTS_H
#define SORTS_H

#include <atomic>
#include "SortState.h"
#include "CountingVector.h"

void bubbleSort(SortState &state, int sortingDelay);
void selectionSort(SortState &state, int sortingDelay);
void insertionSort(SortState &state, int sortingDelay);

void merge(SortState &state, int sortingDelay, int left, int mid, int right);
void mergeHelper(SortState &state, int sortingDelay, int left, int right);
void mergeSort(SortState &state, int sortingDelay);

#endif // SORTS_H
