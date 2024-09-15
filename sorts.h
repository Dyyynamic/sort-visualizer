#ifndef SORTS_H
#define SORTS_H

#include "SortState.h"
#include "CountingVector.h"

void bubbleSort(SortState &state, int sortingDelay);
void selectionSort(SortState &state, int sortingDelay);
void insertionSort(SortState &state, int sortingDelay);

#endif // SORTS_H
