#pragma once

#include <memory>
#include <functional>
#include <string>

#include "ActionChangeCardinal.hpp"
#include "../Sequences/sequences/Sequence.hpp"

template< class T >
class LazySequence;

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
class MainGen : public IGenerator<T>
{       
        private:
        std::shared_ptr<Sequence<T>> list;
        std::function<T(std::shared_ptr<Sequence<T>> list)> rule;
        bool isInfinite;

        public:
        MainGen( std::shared_ptr<Sequence<T>> list ) 
                                : list(list), rule(nullptr), isInfinite(false) {}
        MainGen( std::shared_ptr<Sequence<T>> list, 
                        std::function<T(std::shared_ptr<Sequence<T>> list)> rule ) 
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
        Cardinal GetLength() override { 
                return isInfinite ? Cardinal::Infinite() : Cardinal( list->GetLength() ); 
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
        Cardinal GetLength() override { return Cardinal(length); }
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
        Cardinal GetLength() override { 
                if( prevGen1->GetLength().IsInfinite() || prevGen2->GetLength().IsInfinite() ) {
                        return Cardinal::Infinite();
                } 
                return ( prevGen1->GetLength().GetSize() + prevGen2->GetLength().GetSize() );
        }
        size_t GetMaterializedCount() const override { 
                return prevGen1->GetMaterializedCount() + prevGen2->GetMaterializedCount(); 
        };
};