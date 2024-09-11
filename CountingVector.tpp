#include "CountingVector.h"

#include <algorithm>

template <typename T>
CountingVector<T>::CountingVector() : accessCount(0), countAccesses(false) {}

template <typename T>
T &CountingVector<T>::operator[](size_t index)
{
    if (countAccesses)
        ++accessCount;
    return vec[index];
}

template <typename T>
void CountingVector<T>::swap(CountingVector<T> &other)
{
    std::swap(vec, other.vec);
    std::swap(accessCount, other.accessCount);
}

template <typename T>
void CountingVector<T>::push_back(const T &value)
{
    vec.push_back(value);
}

template <typename T>
typename std::vector<T>::iterator CountingVector<T>::begin()
{
    return vec.begin();
}

template <typename T>
typename std::vector<T>::iterator CountingVector<T>::end()
{
    return vec.end();
}

template <typename T>
int CountingVector<T>::size() const
{
    return vec.size();
}

template <typename T>
int CountingVector<T>::getAccessCount() const
{
    return accessCount;
}

template <typename T>
void CountingVector<T>::setAccessCount(int value)
{
    accessCount = value;
}

template <typename T>
void CountingVector<T>::setAccessCounting(bool value)
{
    countAccesses = value;
}
