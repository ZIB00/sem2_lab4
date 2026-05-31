#pragma once

#include <cstddef>
#include <functional>
#include <vcruntime_typeinfo.h>

#include "../../Sequences/sequences/Sequence.hpp"
#include "../../Sequences/sequences/ArraySequence.hpp"
#include "../../Sequences/sequences/PriorityQueue.hpp"
#include "../../Sequences/other/Option.hpp"

enum Operations {
        Insert,
        Remove
};

template<class T>
struct Change {
        size_t targetIndex;
        T value;
        Operations operation;
};

class Cardinal {
    bool isInfinite; 
    size_t value; 

    Cardinal(bool isInfinite, size_t val) : isInfinite(isInfinite), value(val) {}

    public:

    Cardinal(size_t val) : Cardinal(false, val) {}
    static Cardinal Infinite() { return Cardinal(true, 0); }

    bool IsInfinite() const { return isInfinite; }
    size_t GetValue() const { return value; }
};

template< class T >
class LazySequence : public Sequence<T>
{
        private:
        class Generator
        {
                private:
                LazySequence<T>* parent;
                std::function<T(Sequence<T>*)> rule;

                unsigned currentIndex;
                PriorityQueue<Change<T>, std::greater<T>> queue;

                public:
                Generator( LazySequence<T>* list, std::function<T(Sequence<T>*)> func ) : parent(list), rule(func), currentIndex(0), queue(new PriorityQueue<T>) {} 
                Generator( LazySequence<T>* list, const Generator& gen, const Change<T>& change );
                Generator( LazySequence<T>* list, const Generator& other ) : parent(list), rule(other.rule) {}
                ~Generator() { if(parent) delete parent; }

                T GetNext();
                bool HasNext() const;
                Option<T> TryGetNext();

                Generator* Append(T item)             const;
                Generator* Append(Sequence<T>* items) const;
                Generator* Insert(T item)             const;
                Generator* Insert(Sequence<T>* items) const;
                Generator* Remove(T item)             const;
                Generator* Remove(Sequence<T>* items) const;
        };

        bool isInfinite;
        MutableArraySequence<T> list;
        Generator* generator;

        public:
        using value_type = T;

        LazySequence() : isInfinite(false), list(), generator(nullptr) {}
        LazySequence( T* items, int count ) : isInfinite(false), list( items, static_cast<size_t>(count) ), generator(nullptr) {}
        LazySequence( Sequence<T>* seq )    : isInfinite(false), list(seq), generator(nullptr) {}
        //LazySequence( T(*func)(Sequence<T>*), Sequence<T>* seq ) : isInfinite(true), list(seq), generator(new Generator(this, func)) {} Зачем?
        LazySequence( std::function<T(Sequence<T>*)> func, Sequence<T>* seq ) : isInfinite(true), list(seq), generator(nullptr) { if(func) this->generator = new Generator(this, func); }
        LazySequence( const LazySequence<T>& list ) : isInfinite(list.isInfinite), list(list.list), generator(nullptr) { if(list.generator) this->generator = new Generator(this, *(list.generator)); }
        ~LazySequence() { if (generator) delete generator; };

        T GetFirst() override;
        T GetLast() override;
        const T& Get(size_t index) override;
        void Set( size_t index, T value) override;
        LazySequence<T>* GetSubsequence( size_t startIndex, size_t endIndex ) override;
        Cardinal GetLength() override;
        size_t GetMaterializedCount() const; 

        Sequence<T>* Append(T item) override;
        Sequence<T>* Prepend(T item) override;
        Sequence<T>* InsertAt( T item, size_t index ) override;
        LazySequence<T>* Concat( LazySequence<T>* list ) override;

        Sequence<T>* CreateEmpty() override;
        
        IEnumerator<T>* GetEnumerator() override;
};

#include "../implementation/Generator.tpp"
#include "../implementation/LazySequence.tpp"