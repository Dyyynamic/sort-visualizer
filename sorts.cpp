#include "sorts.h"
#include <thread>

void bubbleSort(
    CountingVector<int> &numbers,
    std::mutex &mtx,
    int &comparisons,
    int sortingDelay,
    bool &sortingComplete)
{
    for (int i = 0; i < numbers.size(); i++)
    {
        for (int j = 0; j < numbers.size() - i - 1; j++)
        {
            std::unique_lock<std::mutex> lock(mtx);

            numbers.enableAccessCounting();

            if (numbers[j] > numbers[j + 1])
                std::swap(numbers[j], numbers[j + 1]);

            comparisons++;

            numbers.disableAccessCounting();

            // Unlock before sleeping
            lock.unlock();

            std::this_thread::sleep_for(std::chrono::microseconds(sortingDelay));
        }
    }

    sortingComplete = true;
}

void selectionSort(
    CountingVector<int> &numbers,
    std::mutex &mtx,
    int &comparisons,
    int sortingDelay,
    bool &sortingComplete)
{
    for (int i = 0; i < numbers.size() - 1; i++)
    {
        int minIndex = i;

        for (int j = i + 1; j < numbers.size(); j++)
        {
            std::unique_lock<std::mutex> lock(mtx);

            numbers.enableAccessCounting();

            if (numbers[j] < numbers[minIndex])
                minIndex = j;

            comparisons++;

            numbers.disableAccessCounting();

            // Unlock before sleeping
            lock.unlock();

            std::this_thread::sleep_for(std::chrono::microseconds(sortingDelay));
        }

        if (i != minIndex)
        {
            std::lock_guard<std::mutex> lock(mtx);

            numbers.enableAccessCounting();

            std::swap(numbers[i], numbers[minIndex]);
            comparisons++;

            numbers.disableAccessCounting();
        }
    }

    sortingComplete = true;
}

void insertionSort(
    CountingVector<int> &numbers,
    std::mutex &mtx,
    int &comparisons,
    int sortingDelay,
    bool &sortingComplete)
{
    // BUG: In the visualization, this sort causes popping, seemingly caused by the entire vector
    //      being accessed at onece. It is not clear to me why this happens.

    for (int i = 1; i < numbers.size(); i++)
    {
        std::unique_lock<std::mutex> lock(mtx);
        numbers.enableAccessCounting();

        int temp = numbers[i];

        numbers.disableAccessCounting();
        lock.unlock();

        int j = i - 1;

        // Using a while true loop so the mutex is locked before checking
        while (true)
        {
            std::unique_lock<std::mutex> lock(mtx);

            numbers.enableAccessCounting();

            if (j < 0 || temp >= numbers[j])
                break;

            numbers[j + 1] = numbers[j];
            comparisons++;

            j -= 1;

            numbers.disableAccessCounting();

            // Unlock before sleeping
            lock.unlock();

            std::this_thread::sleep_for(std::chrono::microseconds(sortingDelay));
        }

        lock.lock();
        numbers.enableAccessCounting();

        numbers[j + 1] = temp;

        numbers.disableAccessCounting();
        lock.unlock();
    }

    sortingComplete = true;
}
