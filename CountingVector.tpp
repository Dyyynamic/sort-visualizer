#include "CountingVector.h"

#include <algorithm>

template <typename T>
CountingVector<T>::CountingVector() : accessCount(0), countAccesses(false) {}

template <typename T>
T &CountingVector<T>::operator[](size_t index)
{
    if (countAccesses)
    {
        ++accessCount;
        accessed[index] = true;
    }

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

    accessed.push_back(false);
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
bool CountingVector<T>::isAccessed(int index) const
{
    return accessed[index];
}

template <typename T>
void CountingVector<T>::clearAccessed(int index)
{
    accessed[index] = false;
}

template <typename T>
void CountingVector<T>::enableAccessCounting()
{
    countAccesses = true;
}

template <typename T>
void CountingVector<T>::disableAccessCounting()
{
    countAccesses = false;
}

template <typename T>
bool CountingVector<T>::getCountAccesses() const
{
    return countAccesses;
}
