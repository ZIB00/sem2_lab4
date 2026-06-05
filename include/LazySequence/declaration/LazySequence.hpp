#pragma once

#include <cstddef>
#include <functional>
#include <vcruntime_typeinfo.h>
#include <memory>

#include "../../Sequences/sequences/Sequence.hpp"
#include "../../Sequences/sequences/ArraySequence.hpp"

enum Action {
        INSERT,
        SET,
        REMOVE
};

template<class T>
struct Change {
        size_t targetIndex;
        T value;
        Action action;

        bool operator<(const Change<T>& other) const {
                return this->targetIndex < other.targetIndex;
        }
};

class Cardinal {
    private:

    bool isInfinite; 
    size_t size; 

    Cardinal(bool isInfinite, size_t val) : isInfinite(isInfinite), size(val) {}

    public:

    Cardinal(size_t size) : Cardinal(false, size) {}
    static Cardinal Infinite() { return Cardinal(true, 0); }

    Cardinal operator+(size_t size) { 
        return isInfinite ? Cardinal::Infinite() : Cardinal(false, this->size + size); 
        }

    Cardinal operator-(size_t size) { 
        if(isInfinite) return Cardinal::Infinite();
        return Cardinal(false, (this->size >= size) ? this->size - size : 0); 
        }

    bool IsInfinite() const { return isInfinite; }
    size_t GetSize() const { return size; }
};

template<class T>
class IGenerator
{       
        public:
        virtual ~IGenerator() = default;
        virtual const T& Get( size_t index ) = 0;
        virtual Cardinal GetLength() = 0;
};

template<class T>
class MainGenerator : public IGenerator<T>
{       
        private:
        std::shared_ptr<ArraySequence<T>> list;
        std::function<T(std::shared_ptr<Sequence<T>> list)> rule;
        bool isInfinite;

        public:
        MainGenerator( std::shared_ptr<ArraySequence<T>> list, 
                        std::function<T(std::shared_ptr<Sequence<T>> list)> rule ) 
                                : list(list), rule(rule), isInfinite(true) {}

        const T& Get( size_t index ) override {
                if( list->GetLength() > index ) return list->Get(index);

                try {
                        while( list->GetLength() <= index ) {
                                list->Append(rule(list));
                        }

                        return list->Get(index);
                } catch (...) {
                        isInfinite = false;
                        throw OutOfRange( std::string("Index is out of range, sequense is finite, it lenght is: " + 
                                std::to_string(list->GetLength())) );
                }
        }
        Cardinal GetLength() override { 
                return isInfinite ? Cardinal::Infinite() : Cardinal( list->GetLength() ); 
        }
};


template<class T>
class ChangeGenerator : public IGenerator<T>
{
        private:
        std::shared_ptr<IGenerator<T>> prevGen;
        Change<T> change;

        public:
        ChangeGenerator( std::shared_ptr<IGenerator<T>> prevGen, Change<T> change ) 
                                : prevGen(prevGen), change(change) {}

        const T& Get( size_t index ) override {
                if( index == change.targetIndex ) {
                        if( change.action == Action::SET || change.action == Action::INSERT ) {
                                return change.value;
                        }
                        if( change.action == Action::REMOVE ) {
                                return prevGen->Get(index + 1);
                        }
                }
                if( index > change.targetIndex ) {
                        if( change.action == Action::INSERT ) {
                                return prevGen->Get(index - 1);
                        }
                        if( change.action == Action::REMOVE ) {
                                return prevGen->Get(index + 1);
                        }
                }
                return prevGen->Get(index);
        }
        Cardinal GetLength() override {
                Cardinal prevLen = prevGen->GetLength();

                if( change.action == Action::INSERT ) {
                        return prevLen + 1;
                }
                if( change.action == Action::REMOVE ) {
                        if( !prevLen.IsInfinite() && change.targetIndex >= prevLen.GetSize() ) {
                                return prevLen;
                        }
                        return prevLen - 1;
                }
                return prevLen;
        }
};

template< class T >
class LazySequence
{
        private:
        MutableArraySequence<T> list;
        std::shared_ptr<IGenerator<T>> generator;

        LazySequence( const LazySequence<T>& list, std::shared_ptr<IGenerator<T>> newGen ) 
                : list(list.list), generator(newGen) {}

        public:
        using value_type = T;

        LazySequence() : list(), generator(nullptr) {}
        LazySequence( T* items, int count ) : list( items, static_cast<size_t>(count) ), generator(nullptr) {}
        LazySequence( Sequence<T>* seq )    : list(seq), generator(nullptr) {}
        //LazySequence( T(*func)(Sequence<T>*), Sequence<T>* seq ) : isInfinite(true), list(seq), generator(new Generator(this, func)) {} Зачем?
        LazySequence( std::function<T(Sequence<T>*)> func, Sequence<T>* seq ) 
                : list(seq), generator(nullptr) { if(func) this->generator = std::make_shared<IGenerator>(this, func); }
        LazySequence( const LazySequence<T>& list ) 
                : list(list.list), generator(nullptr) { if(list.generator) this->generator = std::make_shared<IGenerator>(this, *(list.generator)); }
        ~LazySequence() = default;

        T GetFirst() { return Get(0); }
        T GetLast()  { return Get(list.GetLength() - 1); }
        const T& Get(size_t index) { return generator->Get(index); }
        Cardinal GetLength() { return generator->GetLength(); }
        size_t GetMaterializedCount() const { return list.GetLength(); } 
        
        LazySequence<T>* Set( size_t index, T value );
        LazySequence<T>* GetSubsequence( size_t startIndex, size_t endIndex );
        LazySequence<T>* Append(T item);
        LazySequence<T>* Prepend(T item);
        LazySequence<T>* InsertAt( T item, size_t index );
        LazySequence<T>* Concat( LazySequence<T>* list );
};

#include "../implementation/LazySequence.tpp"