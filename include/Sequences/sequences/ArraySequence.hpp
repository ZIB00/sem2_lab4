#pragma once

#include "DynamicArray.hpp"
#include "Sequence.hpp"

template<class T>
class ArraySequence : public Sequence<T>
{
    private:
        DynamicArray<T>* items;

        void ValidateNotEmpty();
        void ValidateSubsequenceRange(size_t startIndex, size_t endIndex);
        void ValidateInsertIndex(size_t index);
        void AppendInternal(T item);
        void InsertAtInternal(T item, size_t index);
        void ConcatInternal(Sequence<T>* list);

    protected:
        ArraySequence();
        void SetItems(T* items, size_t count);
        void SetItems(std::initializer_list<T> items);
        void CopyItems(const ArraySequence<T>& sequence);
        virtual ArraySequence<T>* Instance() = 0;
        virtual ArraySequence<T>* CreateEmptyMut() = 0;

    public:
        ~ArraySequence() override;

        using value_type = T;

        ArraySequence<T>& operator=(const ArraySequence<T>& sequence);
        T& operator[](size_t index);
        const T& operator[](size_t index) const;
        Sequence<T>* operator+(Sequence<T>* other);
        bool operator==(Sequence<T>* other);
        bool operator!=(Sequence<T>* other);

        T GetFirst() override;
        T GetLast() override;
        const T& Get(size_t index) override;
        void Set( size_t index, T value) override;
        Sequence<T>* GetSubsequence(size_t startIndex, size_t endIndex) override;
        size_t GetLength() override;

        Sequence<T>* Append(T item) override;
        Sequence<T>* Prepend(T item) override;
        Sequence<T>* InsertAt(T item, size_t index) override;
        Sequence<T>* Concat(Sequence<T>* list) override;

        Sequence<T>* CreateEmpty() override;

        IEnumerator<T>* GetEnumerator() override;
};

template<class T>
class ArraySequenceEnumerator : public IEnumerator<T>
{
private:
    ArraySequence<T>* sequence;
    int position;

public:
    ArraySequenceEnumerator(ArraySequence<T>* sequence);

    T GetCurrent() override;
    bool MoveNext() override;
    void Reset() override;
};

template<class T>
class MutableArraySequence : public ArraySequence<T>
{
    protected:
        ArraySequence<T>* Instance() override;
        ArraySequence<T>* CreateEmptyMut() override;

    public:
        MutableArraySequence();
        MutableArraySequence(T* items, size_t count);
        MutableArraySequence(const MutableArraySequence<T>& sequence);
        MutableArraySequence(std::initializer_list<T> items);

        MutableArraySequence<T>& operator=(const MutableArraySequence<T>& sequence);
};

template<class T>
class ImmutableArraySequence : public ArraySequence<T>
{
    protected:
        ArraySequence<T>* Instance() override;
        ArraySequence<T>* CreateEmptyMut() override;

    public:
        ImmutableArraySequence();
        ImmutableArraySequence(T* items, size_t count);
        ImmutableArraySequence(const ImmutableArraySequence<T>& sequence);
        ImmutableArraySequence(std::initializer_list<T> items);

        ImmutableArraySequence<T>& operator=(const ImmutableArraySequence<T>& sequence);
};

#pragma region sequence main functions

template<class T>
ArraySequence<T>::ArraySequence()
{
    this->items = new DynamicArray<T>(0);
}

template<class T>
ArraySequence<T>::~ArraySequence()
{
    delete this->items;
}

template<class T>
void ArraySequence<T>::SetItems(T* items, size_t count)
{
    DynamicArray<T>* newItems = new DynamicArray<T>(items, count);
    delete this->items;
    this->items = newItems;
}

template<class T>
void ArraySequence<T>::SetItems(std::initializer_list<T> items)
{
    DynamicArray<T>* newItems = new DynamicArray<T>(items);
    delete this->items;
    this->items = newItems;
}

template<class T>
void ArraySequence<T>::CopyItems(const ArraySequence<T>& sequence)
{
    DynamicArray<T>* copiedItems = new DynamicArray<T>(*sequence.items);
    delete this->items;
    this->items = copiedItems;
}

template<class T>
void ArraySequence<T>::ValidateNotEmpty()
{
    if (this->GetLength() == 0) throw OutOfRange("Sequence is empty");
}

template<class T>
void ArraySequence<T>::ValidateSubsequenceRange(size_t startIndex, size_t endIndex)
{
    if (startIndex < 0) throw InvalidArgument("Start index cannot be negative");
    if (endIndex < 0) throw InvalidArgument("End index cannot be negative");
    if (startIndex > endIndex) throw InvalidArgument("Start index cannot be greater than end index");
    if (endIndex >= this->GetLength()) throw OutOfRange("Index is out of range");
}

template<class T>
void ArraySequence<T>::ValidateInsertIndex(size_t index)
{
    if (index < 0) throw InvalidArgument("Index cannot be negative");
    if (index > this->GetLength()) throw OutOfRange("Index is out of range");
}

template<class T>
void ArraySequence<T>::AppendInternal(T item)
{
    size_t length = this->GetLength();
    this->items->Resize(length + 1);
    this->items->Set(length, item);
}

template<class T>
void ArraySequence<T>::InsertAtInternal(T item, size_t index)
{
    this->ValidateInsertIndex(index);

    size_t length = this->GetLength();
    this->items->Resize(length + 1);

    for (size_t currentIndex = length; currentIndex > index; --currentIndex) {
        this->items->Set(currentIndex, this->items->Get(currentIndex - 1));
    }

    this->items->Set(index, item);
}

template<class T>
void ArraySequence<T>::ConcatInternal(Sequence<T>* list)
{
    if (list == nullptr) throw InvalidArgument("Sequence cannot be null");

    size_t listLength = list->GetLength();

    for (size_t index = 0; index < listLength; ++index) {
        this->AppendInternal(list->Get(index));
    }
}

template<class T>
T ArraySequence<T>::GetFirst()
{
    this->ValidateNotEmpty();
    return this->items->Get(0);
}

template<class T>
T ArraySequence<T>::GetLast()
{
    this->ValidateNotEmpty();
    return this->items->Get(this->GetLength() - 1);
}

template<class T>
const T& ArraySequence<T>::Get(size_t index)
{
    return this->items->Get(index);
}

template<class T>
void ArraySequence<T>::Set(size_t index, T value)
{
    this->items->Set( index, value );
}

template<class T>
Sequence<T>* ArraySequence<T>::GetSubsequence(size_t startIndex, size_t endIndex)
{
    this->ValidateSubsequenceRange(startIndex, endIndex);

    ArraySequence<T>* result = this->CreateEmptyMut();

    try {
        for (size_t index = startIndex; index <= endIndex; ++index) {
            result->AppendInternal(this->items->Get(index));
        }
    }
    catch (...) {
        delete result;
        throw;
    }

    return result;
}

template<class T>
size_t ArraySequence<T>::GetLength()
{
    return this->items->GetSize();
}

template<class T>
Sequence<T>* ArraySequence<T>::Append(T item)
{
    ArraySequence<T>* result = this->Instance();
    result->AppendInternal(item);
    return result;
}

template<class T>
Sequence<T>* ArraySequence<T>::Prepend(T item)
{
    return this->InsertAt(item, 0);
}

template<class T>
Sequence<T>* ArraySequence<T>::InsertAt(T item, size_t index)
{
    ArraySequence<T>* result = this->Instance();
    result->InsertAtInternal(item, index);
    return result;
}

template<class T>
Sequence<T>* ArraySequence<T>::Concat(Sequence<T>* list)
{
    ArraySequence<T>* result = this->Instance();
    result->ConcatInternal(list);
    return result;
}

#pragma endregion

template <class T>
Sequence<T>* ArraySequence<T>::CreateEmpty() {
    return this->CreateEmptyMut();
}

#pragma region operators (=, [], const [], +, ==, !=)

template<class T>
ArraySequence<T>& ArraySequence<T>::operator=(const ArraySequence<T>& sequence)
{
    if (this == &sequence) {
        return *this;
    }

    DynamicArray<T>* copiedItems = new DynamicArray<T>(*sequence.items);
    delete this->items;
    this->items = copiedItems;

    return *this;
}

template<class T>
T& ArraySequence<T>::operator[](size_t index)
{
    return (*this->items)[index]; 
}

template<class T>
const T& ArraySequence<T>::operator[](size_t index) const
{
    return (*this->items)[index]; 
}

template<class T>
Sequence<T>* ArraySequence<T>::operator+(Sequence<T>* other)
{
    if (other == nullptr) throw InvalidArgument("Cannot add null sequence");

    Sequence<T>* result = this->CreateEmptyMut();

    try {
        for (size_t i = 0; i < this->GetLength(); ++i) {
            result = result->Append((*this)[i]);
        }
        for (size_t i = 0; i < other->GetLength(); ++i) {
            result = result->Append(other->Get(i));
        }
    }
    catch (...) {
        delete result;
        throw;
    }

    return result;
}

template<class T>
bool ArraySequence<T>::operator==(Sequence<T>* other)
{
    if (this == other) return true;
    if (other == nullptr || this->GetLength() != other->GetLength()) return false;

    for (size_t i = 0; i < this->GetLength(); ++i) {
        if ((*this)[i] != other->Get(i)) {
            return false;
        }
    }
    return true;
}

template<class T>
bool ArraySequence<T>::operator!=(Sequence<T>* other)
{
    return !(*this == other);
}

#pragma endregion

template<class T>
IEnumerator<T>* ArraySequence<T>::GetEnumerator()
{
    return new ArraySequenceEnumerator<T>(this);
}

#pragma region IEnumerator

template<class T>
ArraySequenceEnumerator<T>::ArraySequenceEnumerator(ArraySequence<T>* sequence)
{
    this->sequence = sequence;
    this->position = -1;
}

template<class T>
T ArraySequenceEnumerator<T>::GetCurrent()
{
    if (this->position < 0 || this->position >= sequence->GetLength()) throw OutOfRange("Enumerator is out of bounds");

    return sequence->Get(position);
}

template<class T>
bool ArraySequenceEnumerator<T>::MoveNext()
{
    if (position + 1 < sequence->GetLength()) {
        position++;
        return true;
    }
    return false;
}

template<class T>
void ArraySequenceEnumerator<T>::Reset()
{
    this->position = -1;
}

#pragma endregion

#pragma region Mutable/Immutable

template<class T>
MutableArraySequence<T>::MutableArraySequence() {}

template<class T>
MutableArraySequence<T>::MutableArraySequence(T* items, size_t count)
{
    this->SetItems(items, count);
}

template<class T>
MutableArraySequence<T>::MutableArraySequence(const MutableArraySequence<T>& sequence)
{
    this->CopyItems(sequence);
}

template<class T>
MutableArraySequence<T>::MutableArraySequence(std::initializer_list<T> items)
{
    this->SetItems(items);
}

template<class T>
MutableArraySequence<T>& MutableArraySequence<T>::operator=(const MutableArraySequence<T>& sequence)
{
    this->ArraySequence<T>::operator=(sequence);
    return *this;
}

template<class T>
ArraySequence<T>* MutableArraySequence<T>::Instance()
{
    return this;
}

template<class T>
ArraySequence<T>* MutableArraySequence<T>::CreateEmptyMut()
{
    return new MutableArraySequence<T>();
}

template<class T>
ImmutableArraySequence<T>::ImmutableArraySequence() {}

template<class T>
ImmutableArraySequence<T>::ImmutableArraySequence(T* items, size_t count)
{
    this->SetItems(items, count);
}

template<class T>
ImmutableArraySequence<T>::ImmutableArraySequence(const ImmutableArraySequence<T>& sequence)
{
    this->CopyItems(sequence);
}

template<class T>
ImmutableArraySequence<T>& ImmutableArraySequence<T>::operator=(const ImmutableArraySequence<T>& sequence)
{
    this->ArraySequence<T>::operator=(sequence);
    return *this;
}

template<class T>
ArraySequence<T>* ImmutableArraySequence<T>::Instance()
{
    return new ImmutableArraySequence<T>(*this);
}

template<class T>
ArraySequence<T>* ImmutableArraySequence<T>::CreateEmptyMut()
{
    return new ImmutableArraySequence<T>();
}

#pragma endregion