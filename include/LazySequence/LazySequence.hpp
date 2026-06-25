#pragma once

#include <cstddef>
#include <initializer_list>
#include <functional>
#include <utility>

#include "../Sequences/sequences/ListSequence.hpp"
#include "../Sequences/other/Option.hpp"
#include "Generator.hpp"
#include "Sequences/other/Exceptions.hpp"

namespace LazySequenceUtilts {};

template< class T >
class LazySequence
{
        private:
        std::shared_ptr<IGenerator<T>> gen;
        size_t currentIndex = 0;

        LazySequence<T> Change( Change<T> change ) const {
                return LazySequence<T>( std::make_shared<ChangeGen<T>>( gen, change ) );
        }

        public:
        using value_type = T;

        LazySequence() : gen( std::make_shared<MainGen<T>>( std::make_shared<MutableListSequence<T>>() ) ) {}
        LazySequence( T* items, int count ) 
                : gen(std::make_shared<MainGen<T>>( std::make_shared<MutableListSequence<T>>( items, count ) )) {}
        LazySequence( std::shared_ptr<Sequence<T>> seq ) : gen( std::make_shared<MainGen<T>>(seq) ) {}
        LazySequence( std::function<T(std::shared_ptr<Sequence<T>>)> func ) 
                : gen( std::make_shared<MainGen<T>>( std::make_shared<MutableListSequence<T>>(), func) ) {}
        LazySequence( std::function<T(std::shared_ptr<Sequence<T>>)> func, std::shared_ptr<Sequence<T>> seq ) 
                : gen( std::make_shared<MainGen<T>>( seq, func ) ) {}
        LazySequence( std::shared_ptr<IGenerator<T>> gen ) : gen(gen) {}
        LazySequence( std::function<T(std::shared_ptr<Sequence<T>>)> func, std::initializer_list<T> items ) 
                : gen( std::make_shared<MainGen<T>>( std::make_shared<MutableListSequence<T>>(items), func ) ) {}
        
        template< class T2 = T >
        LazySequence( std::shared_ptr<LazySequence<T>> seq, std::function<T2(T)> func )
                : gen( std::make_shared<MapGen<T, T2>>( seq->gen, func ) ) {}

        ~LazySequence() = default;

        LazySequence<T> operator+(const LazySequence<T>& list ) const { return Concat(list); }

        T GetFirst() const { return Get(0); }
        T GetLast()  const;
        const T& Get(size_t index)    const { return gen->Get(index); }
        Ordinal GetLength()          const { return gen->GetLength(); }
        size_t GetMaterializedCount() const { return gen->GetMaterializedCount(); } 

        T GetNext() { return Get(currentIndex++);} 
        Option<T> TryGetNext() { 
                try {
                        return Option<T>(GetNext());
                } catch(...) { 
                        return Option<T>(); 
                }
        }
        
        LazySequence<T> Set      ( T item, size_t index ) const { return Change( {index, item, Action::SET}); }
        LazySequence<T> Append   ( T item )               const;
        LazySequence<T> Prepend  ( T item )               const { return Change( {0    , item, Action::INSERT} ); }
        LazySequence<T> InsertAt ( T item, size_t index ) const { return Change( {index, item, Action::INSERT} ); }
        LazySequence<T> Remove   ( size_t index )         const { return Change( {index, T() , Action::REMOVE} ); }
        LazySequence<T> GetSubsequence( size_t startIndex, size_t endIndex ) const;
        LazySequence<T> Concat( const LazySequence<T>& list ) const;

        template< class T2 = T >
        LazySequence<T2> Map( std::function<T2(T)> func ) const {
                auto newGen = std::make_shared<MapGen<T, T2>>(this->gen, func);
                return LazySequence<T2>(newGen);
        }
        LazySequence<T> Where( std::function<bool(T)> pred ) const {
                auto newGen = std::make_shared<WhereGen<T>>(this->gen, pred);
                return LazySequence<T>(newGen);
        }

        template< class T2 = T >
        T2 Reduce( std::function<T2(T2, T)> func ) const {
                if( this->GetLength().IsInfinite() ) throw LogicError("Reduce fail: Infinite sequence");
                if( this->GetLength().GetSize() == 0) throw InvalidArgument("Reduce fail: sequence is empty");
                if( !func ) throw InvalidArgument("Reduce fail: std::function is empty");

                T2 res = static_cast<T2>(this->Get(0));
                for( size_t i = 1; i < this->GetLength().GetSize(); ++i) {
                        res = func(res, this->Get(i));
                }
                return res;
        }

        template <class U1, class U2>
        friend LazySequence<std::pair<U1, U2>> Zip(std::shared_ptr<LazySequence<U1>>, std::shared_ptr<LazySequence<U2>>);
};

template< class T >
T LazySequence<T>::GetLast() const {
        if( gen->GetLength().IsInfinite() ) throw OutOfRange("Sequence is potentially infinite");
        if( gen->GetLength().GetSize() == 0 ) throw OutOfRange("Sequence is empty");
        
        return Get( gen->GetLength().GetSize() - 1 );
}

template< class T >
LazySequence<T> LazySequence<T>::Append( T item ) const {
        if( gen->GetLength().IsInfinite() ) throw OutOfRange("Sequence is potentially infinite");

        return Change({ gen->GetLength().GetSize(), item, Action::INSERT });
}

template< class T >
LazySequence<T> LazySequence<T>::GetSubsequence( size_t startIndex, size_t endIndex ) const {
        if( startIndex > endIndex ) throw LogicError("end index greater than start index");

        return LazySequence<T>( std::make_shared<SubsequenceGen<T>>( this->gen, startIndex, endIndex ));
}

template< class T >
LazySequence<T> LazySequence<T>::Concat( const LazySequence<T>& list ) const {
        return LazySequence<T>( std::make_shared<ConcatGen<T>>( this->gen, list.gen ));
}

template< class T1, class T2 = T1 >
LazySequence<std::pair<T1, T2>> Zip( std::shared_ptr<LazySequence<T1>> seq1, std::shared_ptr<LazySequence<T2>> seq2 ) {
        auto newGen = std::make_shared<ZipGen<T1, T2>>(seq1->gen, seq2->gen);
        return LazySequence<std::pair<T1, T2>>(newGen);
}