#pragma once

#include <cstddef>
#include <initializer_list>

#include "../Sequences/sequences/ListSequence.hpp"
#include "Generator.hpp"

template< class T >
class LazySequence
{
        private:
        std::shared_ptr<IGenerator<T>> gen;

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
        ~LazySequence() = default;

        T GetFirst() const { return Get(0); }
        T GetLast()  const;
        const T& Get(size_t index)    const { return gen->Get(index); }
        Cardinal GetLength()          const { return gen->GetLength(); }
        size_t GetMaterializedCount() const { return gen->GetMaterializedCount(); } 
        
        LazySequence<T> Set      ( T item, size_t index ) const { return Change( {index, item, Action::SET}); }
        LazySequence<T> Append   ( T item ) const;
        LazySequence<T> Prepend  ( T item )               const { return Change( {0    , item, Action::INSERT} ); }
        LazySequence<T> InsertAt ( T item, size_t index ) const { return Change( {index, item, Action::INSERT} ); }
        LazySequence<T> Remove   ( size_t index )         const { return Change( {index, T() , Action::REMOVE} ); }
        LazySequence<T> GetSubsequence( size_t startIndex, size_t endIndex ) const;
        LazySequence<T> Concat( const LazySequence<T>& list ) const;
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