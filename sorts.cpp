#include "sorts.h"
#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>

void bubbleSort(SortState &state, int sortingDelay, std::atomic<bool> &running)
{
    for (int i = 0; i < state.numbers.size(); i++)
    {
        for (int j = 0; j < state.numbers.size() - i - 1; j++)
        {
            std::unique_lock<std::mutex> lock(state.mtx);

            state.numbers.enableAccessCounting();

            if (state.numbers[j] > state.numbers[j + 1])
                std::swap(state.numbers[j], state.numbers[j + 1]);

            state.comparisons++;

            state.numbers.disableAccessCounting();

            // Unlock before sleeping
            lock.unlock();

            if (!running)
                return;

            std::this_thread::sleep_for(std::chrono::milliseconds(sortingDelay));
        }
    }

    state.sortingComplete = true;
}

void selectionSort(SortState &state, int sortingDelay, std::atomic<bool> &running)
{
    for (int i = 0; i < state.numbers.size() - 1; i++)
    {
        int minIndex = i;

        for (int j = i + 1; j < state.numbers.size(); j++)
        {
            std::unique_lock<std::mutex> lock(state.mtx);

            state.numbers.enableAccessCounting();

            if (state.numbers[j] < state.numbers[minIndex])
                minIndex = j;

            state.comparisons++;

            state.numbers.disableAccessCounting();

            // Unlock before sleeping
            lock.unlock();

            if (!running)
                return;

            std::this_thread::sleep_for(std::chrono::milliseconds(sortingDelay));
        }

        if (i != minIndex)
        {
            std::lock_guard<std::mutex> lock(state.mtx);

            state.numbers.enableAccessCounting();

            std::swap(state.numbers[i], state.numbers[minIndex]);
            state.comparisons++;

            state.numbers.disableAccessCounting();
        }
    }

    state.sortingComplete = true;
}

void insertionSort(SortState &state, int sortingDelay, std::atomic<bool> &running)
{
    for (int i = 1; i < state.numbers.size(); i++)
    {
        std::unique_lock<std::mutex> lock(state.mtx);
        state.numbers.enableAccessCounting();

        int temp = state.numbers[i];

        state.numbers.disableAccessCounting();
        lock.unlock();

        int j = i - 1;

        // Using a while true loop so the mutex is locked before checking
        while (true)
        {
            std::unique_lock<std::mutex> lock(state.mtx);

            state.numbers.enableAccessCounting();

            if (j < 0 || temp >= state.numbers[j])
            {
                // Make sure access counting is disabled before breaking
                // Lock is unlocked automatically when it goes out of scope
                state.numbers.disableAccessCounting();
                break;
            }

            state.numbers[j + 1] = state.numbers[j];
            state.comparisons++;

            j -= 1;

            state.numbers.disableAccessCounting();

            // Unlock before sleeping
            lock.unlock();

            if (!running)
                return;

            std::this_thread::sleep_for(std::chrono::milliseconds(sortingDelay));
        }

        lock.lock();
        state.numbers.enableAccessCounting();

        state.numbers[j + 1] = temp;

        state.numbers.disableAccessCounting();
        lock.unlock();
    }

    state.sortingComplete = true;
}
