#include "../declaration/LazySequence.hpp"
#include <cstddef>
#include <string>

template <class T>
const T& LazySequence<T>::Get(size_t index) {
    if (index < (this->list.GetLength())) return list.Get(index);

    if (!isInfinite || !generator) {
        std::string err{ "Sequence is finite and index is out of range: " + std::to_string(index) + \
            ">" + std::to_string(this->list.GetLength()) };
        throw OutOfRange(err);
    }

    while (list.GetLength() <= index) {
        Option<T> value = generator->TryGetNext();
        if (!value) {
            isInfinite = false;
            std::string err{ "IndexOutOfRange: value cannot be generated anymore" };
            throw OutOfRange(err);
        }
        list.Append(value.Value());
    }

    return list.Get(index);
}

template<class T>
T LazySequence<T>::GetFirst() {
    return Get(0);
}