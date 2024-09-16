#ifndef SORTS_H
#define SORTS_H

#include "SortState.h"

void bubbleSort(SortState &state, int sortingDelay);
void selectionSort(SortState &state, int sortingDelay);
void insertionSort(SortState &state, int sortingDelay);

// Merge sort
void merge(SortState &state, int sortingDelay, int left, int mid, int right);
void mergeHelper(SortState &state, int sortingDelay, int left, int right);
void mergeSort(SortState &state, int sortingDelay);

// Quick sort
int partition(SortState &state, int sortingDelay, int low, int high);
void quickHelper(SortState &state, int sortingDelay, int low, int high);
void quickSort(SortState &state, int sortingDelay);

void bogoSort(SortState &state, int sortingDelay);

#endif // SORTS_H
