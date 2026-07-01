#pragma once

#include <cstddef>
#include "../other/IEnumerator.hpp"

template<class T>
class Sequence
{
    public:
        virtual ~Sequence() = default;

        virtual T GetFirst() = 0;
        virtual T GetLast() = 0;
        virtual const T& Get(size_t index) = 0;
        virtual void Set( size_t index, T value) = 0;
        virtual Sequence<T>* GetSubsequence(size_t startIndex, size_t endIndex) = 0;
        virtual size_t GetLength() = 0;

        virtual Sequence<T>* Append(T item) = 0;
        virtual Sequence<T>* Prepend(T item) = 0;
        virtual Sequence<T>* InsertAt(T item, size_t index) = 0;
        virtual Sequence<T>* Concat(Sequence<T>* list) = 0;

        virtual Sequence<T>* CreateEmpty() = 0;
        
        virtual IEnumerator<T>* GetEnumerator() = 0;
};