#include "../declaration/LazySequence.hpp"
#include <cstddef>
#include <string>

template<class T>
LazySequence<T>::Generator::Generator( LazySequence<T>* list, const Generator& gen, const Change<T>& change ) 
                    : parent(list), queue(gen.queue), rule(gen.rule), currentIndex(gen.currentIndex) {
    queue.Push(change);
}

template<class T>
T LazySequence<T>::Generator::GetNext() {
    if( !(queue.Empty()) && (queue.Top().targetIndex == currentIndex) ) {
        if( queue.Top().action == Action::INSERT ) {
            currentIndex++;
            T value = queue.Top().value;
            queue.pop();
            return value;
        }

        if( queue.Top().action == Action::REMOVE ) {
            queue.pop();
            currentIndex++;
            return GetNext();
        }
    }
    T value;
    try {
        value = rule(parent);
        currentIndex++;
    } catch(...) {
        throw OutOfRange("Function didn't return any result");
    }
    return value;
}

template<class T>
Option<T> LazySequence<T>::Generator::TryGetNext() {
    if( !(queue.Empty()) && (queue.Top().targetIndex == currentIndex) ) {
        if( queue.Top().action == Action::INSERT ) {
            currentIndex++;
            T value = queue.Top().value;
            queue.pop();
            return Option<T>(value);
        }

        if( queue.Top().action == Action::REMOVE ) {
            queue.pop();
            currentIndex++;
            return TryGetNext();
        }
    }
    T value;
    try {
        value = rule(parent);
        currentIndex++;
        return Option<T>(value);
    } catch(...) {
        return Option<T>(Option<T>::nullopt_t);
    }
}

template<class T>
bool LazySequence<T>::Generator::HasNext() const {
    if( !(queue.Empty()) && (queue.Top().targetIndex == currentIndex) ) {
    //     if( queue.Top().action == Action::INSERT ) {
    //         T value = queue.Top().value;
             return true;
    //     }

    //     if( queue.Top().action == Action::REMOVE ) {
            
    //         PriorityQueue<T> newQueue = queue;
    //         size_t newIndex = currentIndex;
    //         while ( newQueue.Top().action == Action::REMOVE ) {
    //             newQueue.pop();
    //             newIndex++;
    //         }
    //         if( newQueue.Top().action == Action::INSERT ) return true;
            
    //         return true; // Мы не можем знать заранее, так что полагаем, что да
    //     }
    }
    // T value;
    // try {
    //     value = rule(parent);
    //     return true;
    // } catch(...) {
    //     return false;
    // }
    // Короче, я не знаю, оставим так
    return parent->isInfinite;
}

template<class T>
LazySequence<T>::Generator* LazySequence<T>::Generator::Insert(size_t index, Sequence<T>* items) const {
    Generator* newGen = new Generator(parent, *this);

    for( size_t i = 0; i < items->GetLength(); ++i ) {
        newGen->queue.Push({ index + i, items->Get(i), Action::INSERT });
    }

    return newGen;
}

template<class T>
LazySequence<T>::Generator* LazySequence<T>::Generator::Append(Sequence<T>* items) const 
{
    return Insert(currentIndex, items);
}

template<class T>
LazySequence<T>::Generator* LazySequence<T>::Generator::Remove(size_t index, Sequence<T>* items) const {
    Generator* newGen = new Generator(parent, *this);

    for( size_t i = 0; i < items->GetLength(); ++i ) {
        newGen->queue.Push({ index + i, items->Get(i), Action::REMOVE });
    }

    return newGen;
}