#pragma once

#include "../other/ICollection.hpp"
#include "../other/IEnumerable.hpp"

template <class T1, class T2>
struct Pair {
    T1 first;
    T2 second;

    Pair() {
        first = T1();
        second = T2();
    } 
    
    Pair(T1 f, T2 s) {
        first = f;
        second = s;
    }

    bool operator==(const Pair<T1, T2>& other) const {
        return first == other.first && second == other.second;
    }

    bool operator!=(const Pair<T1, T2>& other) const {
        return !(*this == other);
    }
};

template<class T>
class Sequence : public ICollection<T>, public IEnumerable<T>
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