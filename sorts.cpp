#include "sorts.h"

bool bubbleSort(SortState &state)
{
    if (state.numbers[state.cursorPos] > state.numbers[state.cursorPos + 1])
        std::swap(state.numbers[state.cursorPos], state.numbers[state.cursorPos + 1]);

    state.comparisons++;
    state.cursorPos++;

    if (state.cursorPos == state.sortedIndex)
    {
        // Finished sorting
        if (state.cursorPos == 1)
            return true;

        state.cursorPos = 0;
        state.sortedIndex -= 1;
    }

    return false;
}

bool selectionSort(SortState &state)
{

    if (state.numbers[state.cursorPos] < state.numbers[state.currentMinIndex])
        state.currentMinIndex = state.cursorPos;

    state.comparisons++;
    state.cursorPos++;

    if (state.cursorPos == state.numbers.size())
    {
        std::swap(state.numbers[state.sortedIndex], state.numbers[state.currentMinIndex]);
        state.sortedIndex++;
        state.cursorPos = state.sortedIndex;
        state.currentMinIndex = state.sortedIndex;

        if (state.sortedIndex == state.numbers.size() - 1)
            return true;
    }

    return false;
}

bool insertionSort(SortState &state)
{
    if (state.numbers[state.cursorPos] < state.numbers[state.cursorPos - 1])
    {
        std::swap(state.numbers[state.cursorPos], state.numbers[state.cursorPos - 1]);
        state.cursorPos--;
    }
    else
    {
        state.sortedIndex++;

        // Avoid moving cursor off the end
        if (state.sortedIndex < state.numbers.size() - 1)
            state.cursorPos = state.sortedIndex + 1;
    }

    state.comparisons++;

    return state.sortedIndex == state.numbers.size() - 1;
}
