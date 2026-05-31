#pragma once

#include "DynamicArray.hpp"
#include <functional>
#include <utility>

template< class T, class Compare = std::less<T> >
class BinaryHeap 
{
    private:
        DynamicArray<T> data;
        Compare comp;

        size_t Parent(size_t i) const     { return (i - 1) / 2; }
        size_t LeftChild(size_t i) const  { return 2 * i + 1; }
        size_t RightChild(size_t i) const { return 2 * i + 2; }

        void SiftUp(size_t index);
        void SiftDown(size_t index);

    public:
        BinaryHeap() : data(0), comp(Compare()) {}
        explicit BinaryHeap( const Compare& compare ) : data(0), comp(compare) {}
        BinaryHeap( const Compare& compare, const DynamicArray<T>& array );
        BinaryHeap( const Compare& compare, DynamicArray<T>&& array) ;
        BinaryHeap( const BinaryHeap< T, Compare >& other ) : data(other.data), comp(other.comp) {}
        BinaryHeap( BinaryHeap< T, Compare >&& other );

        BinaryHeap< T, Compare >& operator=( const BinaryHeap< T, Compare >& other );
        BinaryHeap< T, Compare >& operator=( BinaryHeap< T, Compare >&& other );

        size_t GetSize() const   { return data.GetSize(); }
        bool IsEmpty() const     { return data.GetSize() == 0; }
        const T& GetRoot() const { return data.Get(0); }

        void Insert(const T& item);
        void Insert(T&& item);
        void ExtractRoot();
        void Swap(BinaryHeap< T, Compare >& other);
};

template<class T, class Compare>
void BinaryHeap<T, Compare>::SiftUp(size_t index)
{
    while (index > 0) {
        size_t p = Parent(index);
        
        if (comp(data.Get(p), data.Get(index))) {
            T temp = data.Get(p);
            data.Set(p, data.Get(index));
            data.Set(index, temp);
            
            index = p;
        } else {
            break; 
        }
    }
}

template<class T, class Compare>
void BinaryHeap<T, Compare>::SiftDown(size_t index)
{
    size_t length = data.GetSize();
    
    while (LeftChild(index) < length) {
        size_t left = LeftChild(index);
        size_t right = RightChild(index);
        size_t largest = index;

        if (comp(data.Get(largest), data.Get(left))) {
            largest = left;
        }
        
        if (right < length && comp(data.Get(largest), data.Get(right))) {
            largest = right;
        }

        if (largest == index) {
            break;
        }

        T temp = data.Get(index);
        data.Set(index, data.Get(largest));
        data.Set(largest, temp);
        
        index = largest;
    }
}

template<class T, class Compare>
BinaryHeap<T, Compare>::BinaryHeap(const Compare& compare, const DynamicArray<T>& array)
    : data(array), comp(compare)
{
    if (data.GetSize() > 1) {
        long long lastParent = (static_cast<long long>(data.GetSize()) / 2) - 1;
        
        for (long long i = lastParent; i >= 0; --i) {
            SiftDown(static_cast<size_t>(i));
        }
    }
}

template<class T, class Compare>
BinaryHeap<T, Compare>::BinaryHeap(const Compare& compare, DynamicArray<T>&& array)
    : data(std::move(array)), comp(compare)
{
    array.Resize(0);
    
    if (data.GetSize() > 1) {
        long long lastParent = (static_cast<long long>(data.GetSize()) / 2) - 1;
        for (long long i = lastParent; i >= 0; --i) {
            SiftDown(static_cast<size_t>(i));
        }
    }
}

template<class T, class Compare>
BinaryHeap<T, Compare>::BinaryHeap(BinaryHeap<T, Compare>&& other) 
    : data(std::move(other.data)), comp(std::move(other.comp)) 
{
    other.data.Resize(0);
}

template<class T, class Compare>
BinaryHeap< T, Compare >& BinaryHeap< T, Compare >::operator=(const BinaryHeap< T, Compare >& other)
{
    if (this != &other) {
        data = std::move(other.data);
        comp = std::move(other.comp);
    }
    return *this;
}

template<class T, class Compare>
BinaryHeap<T, Compare>& BinaryHeap<T, Compare>::operator=(BinaryHeap<T, Compare>&& other)
{
    if (this != &other) {
        this->data = std::move(other.data);
        this->comp = std::move(other.comp);
        
        other.data.Resize(0);
    }
    return *this;
}

template<class T, class Compare>
void BinaryHeap< T, Compare >::Insert(const T& item) {
    size_t currentSize = data.GetSize();
    data.Resize(currentSize + 1);
    data.Set(currentSize, item);
    SiftUp(currentSize);
}

template<class T, class Compare>
void BinaryHeap< T, Compare >::Insert(T&& item) {
    size_t currentSize = data.GetSize();
    data.Resize(currentSize + 1);
    data.Set(currentSize, std::move(item));
    SiftUp(currentSize);
}

template<class T, class Compare>
void BinaryHeap< T, Compare >::ExtractRoot() {
    size_t currentSize = data.GetSize();
    if (currentSize == 0) return;

    if (currentSize > 1) {
        data.Set(0, data.Get(currentSize - 1));
    }
    data.Resize(currentSize - 1);

    if (data.GetSize() > 0) {
        SiftDown(0);
    }
}

template<class T, class Compare>
void BinaryHeap< T, Compare >::Swap(BinaryHeap< T, Compare >& other) {
    std::swap(this->data, other.data);
    std::swap(this->comp, other.comp);
}