#pragma once

#include <cstddef>
#include <memory>
#include <functional>
#include <string>
#include <utility>

#include "ActionChangeOrdinal.hpp"
#include "../Sequences/sequences/Sequence.hpp"
#include "Sequences/sequences/ListSequence.hpp"

template< class T >
class LazySequence;

template<class T>
class IGenerator
{       
        public:
        virtual ~IGenerator() = default;
        virtual const T& Get( size_t index ) = 0;
        virtual Ordinal GetLength() = 0;
        virtual size_t GetMaterializedCount() const = 0;
};

template<class T, class T2 = T>
class MainGen : public IGenerator<T2>
{       
        private:
        std::shared_ptr<Sequence<T>> list;
        std::function<T2(std::shared_ptr<Sequence<T>> list)> rule;
        bool isInfinite;

        public:
        MainGen( std::shared_ptr<Sequence<T>> list ) 
                                : list(list), rule(nullptr), isInfinite(false) {}
        MainGen( std::shared_ptr<Sequence<T>> list, 
                        std::function<T2(std::shared_ptr<Sequence<T>> list)> rule ) 
                                : list(list), rule(rule), isInfinite(true) {}

        const T& Get( size_t index ) override {
                if( list->GetLength() > index ) return list->Get(index);
                if( !rule ) throw OutOfRange( std::string("Index is out of range, sequense is finite, it lenght is: " + 
                                std::to_string(list->GetLength())) );

                try {
                        while( list->GetLength() <= index ) {
                                list->Append(rule(list));
                        }

                        return list->Get(index);
                } catch (...) {
                    isInfinite = false;
                    throw OutOfRange("Sequence is finite. Reached end at: " + std::to_string(list->GetLength()));
            }
        }
        Ordinal GetLength() override { 
                return isInfinite ? Ordinal::Infinite() : Ordinal( list->GetLength() ); 
        }
        size_t GetMaterializedCount() const { return list->GetLength(); }
};


template<class T>
class ChangeGen : public IGenerator<T>
{
        private:
        std::shared_ptr<IGenerator<T>> prevGen;
        Change<T> change;

        public:
        ChangeGen( std::shared_ptr<IGenerator<T>> prevGen, Change<T> change ) 
                                : prevGen(prevGen), change(change) {}

        const T& Get( size_t index ) override {
            if( index >= change.targetIndex && change.action == Action::REMOVE ) return prevGen->Get(index + 1);
            if( index == change.targetIndex && 
                ( change.action == Action::SET || change.action == Action::INSERT )) return change.value;
            if( index > change.targetIndex && change.action == Action::INSERT) return prevGen->Get(index - 1);
            return prevGen->Get(index);
        }
        Ordinal GetLength() override {
            Ordinal prevLen = prevGen->GetLength();

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
class SubsequenceGen : public IGenerator<T>
{       
        private:
        std::shared_ptr<IGenerator<T>> prevGen;
        size_t startIndex;
        size_t length;

        public:
        SubsequenceGen( std::shared_ptr<IGenerator<T>> prevGen, size_t startIndex, size_t endIndex) 
                : prevGen(prevGen), startIndex(startIndex), length(endIndex - startIndex + 1) {}

        const T& Get( size_t index ) override {
                if( index >= length ) throw OutOfRange( std::string("Subsequence size is" + std::to_string(length)) );

                return prevGen->Get( index + startIndex );
        }
        Ordinal GetLength() override { return Ordinal(length); }
        size_t GetMaterializedCount() const override { return prevGen->GetMaterializedCount(); };
};

template< class T >
class ConcatGen : public IGenerator<T>
{       
        private:
        std::shared_ptr<IGenerator<T>> prevGen1;
        std::shared_ptr<IGenerator<T>> prevGen2;

        public:
        ConcatGen( std::shared_ptr<IGenerator<T>> prevGen1, std::shared_ptr<IGenerator<T>> prevGen2)
                : prevGen1(prevGen1), prevGen2(prevGen2) {}

        const T& Get( size_t index ) override {
                if( prevGen1->GetLength().IsInfinite() ) return prevGen1->Get(index);

                size_t len = prevGen1->GetLength().GetSize();
                return (index < len) ? prevGen1->Get(index) : prevGen2->Get(index - len);
        }
        Ordinal GetLength() override { 
                if( prevGen1->GetLength().IsInfinite() || prevGen2->GetLength().IsInfinite() ) {
                        return Ordinal::Infinite();
                } 
                return ( prevGen1->GetLength().GetSize() + prevGen2->GetLength().GetSize() );
        }
        size_t GetMaterializedCount() const override { 
                return prevGen1->GetMaterializedCount() + prevGen2->GetMaterializedCount(); 
        };
};

template<class T, class T2 = T>
class MapGen : public IGenerator<T2>
{       
        private:
        std::shared_ptr<IGenerator<T>> prevGen;
        std::shared_ptr<MutableListSequence<T2>> list;
        std::function< T2(T) > rule;

        public:
        MapGen( std::shared_ptr<IGenerator<T>> prevGen, std::function< T2(T) > func ) 
                : prevGen(prevGen), list(std::make_shared<MutableListSequence<T2>>()), rule([=](size_t index) { return func(prevGen->Get(index));}) {}

        const T2& Get( size_t index ) override {
                if( list->GetLength() > index ) return list->Get(index);

                while( list->GetLength() <= index ) {
                        list->Append(rule(index));
                }

                return list->Get(index);
        }
        Ordinal GetLength()                  { return prevGen->GetLength(); }
        size_t   GetMaterializedCount() const { return prevGen->GetMaterializedCount(); }
};

template<class T>
class WhereGen : public IGenerator<T>
{       
private:
    std::shared_ptr<IGenerator<T>> prevGen;
    std::shared_ptr<MutableListSequence<T>> list;
    std::function< bool(T) > pred;
    size_t prevIndex;

public:
    WhereGen( std::shared_ptr<IGenerator<T>> prevGen, std::function< bool(T) > pred ) 
        : prevGen(prevGen), list(std::make_shared<MutableListSequence<T>>()), pred(pred), prevIndex(0) {}

    const T& Get( size_t index ) override {
        if( list->GetLength() > index ) return list->Get(index);

        while( list->GetLength() <= index ) {
            if( !prevGen->GetLength().IsInfinite() && prevIndex >= prevGen->GetLength().GetSize() ) {
                throw OutOfRange("Index out of bounds in WhereGen");
            }

            T item = prevGen->Get(prevIndex++);
            if( pred(item) ) {
                list->Append(item);
            }
        }

        return list->Get(index);
    }

    Ordinal GetLength() override { throw LogicError("Where does not know the result length"); }
    size_t GetMaterializedCount() const override { return list->GetLength(); }
};

template< class T, class T2 >
class ZipGen : public IGenerator<std::pair<T, T2>>
{       
private:
    std::shared_ptr<IGenerator<T>> prevGen1;
    std::shared_ptr<IGenerator<T2>> prevGen2;
    std::shared_ptr<MutableListSequence<std::pair<T, T2>>> list;

public:
    ZipGen(std::shared_ptr<IGenerator<T>> prevGen1, std::shared_ptr<IGenerator<T2>> prevGen2)
        : prevGen1(prevGen1), prevGen2(prevGen2), list(std::make_shared<MutableListSequence<std::pair<T, T2>>>()) {}

    const std::pair<T, T2>& Get( size_t index ) override { 
        if( list->GetLength() > index ) return list->Get(index);

        while( list->GetLength() <= index ) {
            list->Append(std::make_pair(prevGen1->Get(list->GetLength()), prevGen2->Get(list->GetLength())));
        }

        return list->Get(index); 
    }

    Ordinal GetLength() override { 
        if( prevGen1->GetLength().IsInfinite() && prevGen2->GetLength().IsInfinite() ) return prevGen1->GetLength();
        if( prevGen1->GetLength().IsInfinite() ) return prevGen2->GetLength();
        if( prevGen2->GetLength().IsInfinite() ) return prevGen1->GetLength();
        
        return prevGen1->GetLength().GetSize() < prevGen2->GetLength().GetSize() ? prevGen1->GetLength() : prevGen2->GetLength();
    }

    size_t GetMaterializedCount() const override { 
        return std::min(prevGen1->GetMaterializedCount(), prevGen2->GetMaterializedCount());
    }
};