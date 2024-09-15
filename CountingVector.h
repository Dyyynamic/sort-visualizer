#ifndef COUNTINGVECTOR_H
#define COUNTINGVECTOR_H

#include <vector>

template <typename T>
class CountingVector
{
private:
    std::vector<T> vec;
    std::vector<bool> accessed;
    int accessCount{0};
    bool countAccesses{false};

public:
    CountingVector();

    T &operator[](size_t index);

    void swap(CountingVector<T> &other);
    void push_back(const T &value);

    typename std::vector<T>::iterator begin();
    typename std::vector<T>::iterator end();

    int size() const;
    int getAccessCount() const;
    bool isAccessed(int index) const;
    void clearAccessed(int index);

    void enableAccessCounting();
    void disableAccessCounting();
};

#include "CountingVector.tpp"

#endif // COUNTINGVECTOR_H
