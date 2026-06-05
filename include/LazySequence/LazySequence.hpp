#pragma once

#include <cstddef>
#include <functional>
#include <vcruntime_typeinfo.h>
#include <memory>

#include "../Sequences/sequences/Sequence.hpp"
#include "../Sequences/sequences/ArraySequence.hpp"

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

    Cardinal operator+(size_t size) const { 
        return isInfinite ? Cardinal::Infinite() : Cardinal(false, this->size + size); 
        }

    Cardinal operator-(size_t size) const { 
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
        virtual size_t GetMaterializedCount() const = 0;
};

template<class T>
class MainGenerator : public IGenerator<T>
{       
        private:
        std::shared_ptr<ArraySequence<T>> list;
        std::function<T(std::shared_ptr<Sequence<T>> list)> rule;
        bool isInfinite;

        public:
        MainGenerator( std::shared_ptr<ArraySequence<T>> list ) 
                                : list(list), rule(nullptr), isInfinite(false) {}
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
        size_t GetMaterializedCount() const { return list->GetLength(); }
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
        size_t GetMaterializedCount() const { return prevGen->GetMaterializedCount(); }
};

template< class T >
class LazySequence
{
        private:
        std::shared_ptr<IGenerator<T>> gen;

        LazySequence<T>* Change( Change<T> change ) {
                return new LazySequence( std::shared_ptr(new ChangeGenerator( gen, change )) );
        }

        public:
        using value_type = T;

        LazySequence() : gen(nullptr) {}
        LazySequence( T* items, int count ) 
                : gen(std::make_shared<MainGenerator<T>>( std::make_shared<MutableArraySequence<T>>( items, count ) )) {}
        LazySequence( Sequence<T>* seq ) 
                : gen( std::make_shared<MainGenerator<T>>(std::shared_ptr<Sequence<T>>(seq)) ) {}
        LazySequence( std::function<T(Sequence<T>*)> func, Sequence<T>* seq ) 
                : gen( std::shared_ptr<Sequence<T>>(seq), func ) {}
        LazySequence( std::shared_ptr<IGenerator<T>> gen ) : gen(gen) {}
        ~LazySequence() = default;

        T GetFirst() { return Get(0); }
        T GetLast()  { return Get(gen->GetLength().GetSize() - 1); }
        const T& Get(size_t index) { return gen->Get(index); }
        Cardinal GetLength()       { return gen->GetLength(); }
        size_t GetMaterializedCount() const { return gen->GetMaterializedCount(); } 
        
        LazySequence<T>* Set      ( T item, size_t index ) { return Change( {index, item, Action::SET}); }
        LazySequence<T>* Append   ( T item )               { return Change( {gen->GetLength(), item, Action::INSERT} ); }
        LazySequence<T>* Prepend  ( T item )               { return Change( {0,     item, Action::INSERT} ); }
        LazySequence<T>* InsertAt ( T item, size_t index ) { return Change( {index, item, Action::INSERT} ); }
        LazySequence<T>* Remove   ( size_t index )         { return Change( {index, 0,    Action::REMOVE} ); }
        LazySequence<T>* GetSubsequence( size_t startIndex, size_t endIndex );
        LazySequence<T>* Concat( LazySequence<T>* list );
};

template< class T >
LazySequence<T>* LazySequence<T>::GetSubsequence( size_t startIndex, size_t endIndex ) {
        MutableArraySequence<T> seq;
        try {
                for( size_t i = 0; i < endIndex; ++i ) {
                        seq.Set( i, gen->Get(startIndex + i) );
                }
        } catch(...) {
                throw OutOfRange("Index out of range");
        }
        return new LazySequence<T>(seq);
}

template< class T >
LazySequence<T>* LazySequence<T>::Concat( LazySequence<T>* list ) {
        LazySequence<T> seq;
        try {
                for( size_t i = 0; i < this->gen->GetLength().GetSize(); ++i ) {
                        seq->Append( gen->Set(i) );
                }

                for( size_t i = 0; i < list->gen->GetLength().GetSize(); ++i ) {
                        seq->Append( gen->Set(i) );
                }
        } catch(...) {
                throw OutOfRange("Index out of range");
        }
        return seq;
}