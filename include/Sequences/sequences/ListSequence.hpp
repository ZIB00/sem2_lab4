#pragma once

#include "LinkedList.hpp"
#include "Sequence.hpp"

template<class T>
class ListSequence : public Sequence<T>
{
    private:
        LinkedList<T>* items;

        void AppendInternal(T item);
        void PrependInternal(T item);
        void InsertAtInternal(T item, size_t index);
        void ConcatInternal(Sequence<T>* list);

    protected:
        ListSequence();
        void SetItems(T* items, size_t count);
        void SetItems(std::initializer_list<T> items);
        void CopyItems(const ListSequence<T>& sequence);
        virtual ListSequence<T>* Instance() = 0;
        virtual ListSequence<T>* CreateEmptyMut() = 0;

    public:
        ~ListSequence() override;

        ListSequence<T>& operator=(const ListSequence<T>& sequence);
        T& operator[](size_t index);
        const T& operator[](size_t index) const;
        Sequence<T>* operator+(Sequence<T>* other);
        bool operator==(Sequence<T>* other);
        bool operator!=(Sequence<T>* other);

        T GetFirst() override;
        T GetLast() override;
        const T& Get(size_t index);
        void Set(size_t index, T value);
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
class ListSequenceEnumerator : public IEnumerator<T>
{
    private:
        ListSequence<T>* sequence;
        int position;
    
    public:
        ListSequenceEnumerator(ListSequence<T>* sequence);

        T GetCurrent() override;
        bool MoveNext() override;
        void Reset() override;
};

template<class T>
class MutableListSequence : public ListSequence<T>
{
    protected:
        ListSequence<T>* Instance() override;
        ListSequence<T>* CreateEmptyMut() override;

    public:
        MutableListSequence();
        MutableListSequence(T* items, size_t count);
        MutableListSequence(const MutableListSequence<T>& sequence);
        MutableListSequence(std::initializer_list<T> items);

        MutableListSequence<T>& operator=(const MutableListSequence<T>& sequence);
};

template<class T>
class ImmutableListSequence : public ListSequence<T>
{
    protected:
        ListSequence<T>* Instance() override;
        ListSequence<T>* CreateEmptyMut() override;

    public:
        ImmutableListSequence();
        ImmutableListSequence(T* items, size_t count);
        ImmutableListSequence(const ImmutableListSequence<T>& sequence);
        ImmutableListSequence(std::initializer_list<T> items);

        ImmutableListSequence<T>& operator=(const ImmutableListSequence<T>& sequence);
};

#pragma region sequence main functions

template<class T>
ListSequence<T>::ListSequence()
{
    this->items = new LinkedList<T>();
}

template<class T>
ListSequence<T>::~ListSequence()
{
    delete this->items;
}

template<class T>
void ListSequence<T>::SetItems(T* items, size_t count)
{
    LinkedList<T>* newItems = new LinkedList<T>(items, count);
    delete this->items;
    this->items = newItems;
}

template<class T>
void ListSequence<T>::SetItems(std::initializer_list<T> items)
{
    LinkedList<T>* newItems = new LinkedList<T>(items);
    delete this->items;
    this->items = newItems;
}

template<class T>
void ListSequence<T>::CopyItems(const ListSequence<T>& sequence)
{
    LinkedList<T>* copiedItems = new LinkedList<T>(*sequence.items);
    delete this->items;
    this->items = copiedItems;
}

template<class T>
void ListSequence<T>::AppendInternal(T item)
{
    this->items->Append(item);
}

template<class T>
void ListSequence<T>::PrependInternal(T item)
{
    this->items->Prepend(item);
}

template<class T>
void ListSequence<T>::InsertAtInternal(T item, size_t index)
{
    this->items->InsertAt(item, index);
}

template<class T>
void ListSequence<T>::ConcatInternal(Sequence<T>* list)
{
    if (list == nullptr) throw InvalidArgument("Sequence cannot be null");

    size_t length = list->GetLength();

    for (size_t index = 0; index < length; ++index) {
        this->AppendInternal(list->Get(index));
    }
}

template<class T>
T ListSequence<T>::GetFirst()
{
    return this->items->GetFirst();
}

template<class T>
T ListSequence<T>::GetLast()
{
    return this->items->GetLast();
}

template<class T>
const T& ListSequence<T>::Get(size_t index)
{
    return this->items->Get(index);
}

template<class T>
void ListSequence<T>::Set(size_t index, T value)
{
    this->items->Set( index, value );
}

template<class T>
Sequence<T>* ListSequence<T>::GetSubsequence(size_t startIndex, size_t endIndex)
{
    LinkedList<T>* subList = this->items->GetSubList(startIndex, endIndex);
    ListSequence<T>* result = this->CreateEmptyMut();

    try {
        for (size_t index = 0; index < subList->GetLength(); ++index) {
            result->AppendInternal(subList->Get(index));
        }

        delete subList;
        return result;
    }
    catch (...) {
        delete result;
        delete subList;
        throw;
    }
}

template<class T>
size_t ListSequence<T>::GetLength()
{
    return this->items->GetLength();
}

template<class T>
Sequence<T>* ListSequence<T>::Append(T item)
{
    ListSequence<T>* result = this->Instance();
    result->AppendInternal(item);
    return result;
}

template<class T>
Sequence<T>* ListSequence<T>::Prepend(T item)
{
    ListSequence<T>* result = this->Instance();
    result->PrependInternal(item);
    return result;
}

template<class T>
Sequence<T>* ListSequence<T>::InsertAt(T item, size_t index)
{
    ListSequence<T>* result = this->Instance();
    result->InsertAtInternal(item, index);
    return result;
}

template<class T>
Sequence<T>* ListSequence<T>::Concat(Sequence<T>* list)
{
    ListSequence<T>* result = this->Instance();
    result->ConcatInternal(list);
    return result;
}

#pragma endregion

#pragma region operators (=, [], const [], +, ==, !=)

template<class T>
ListSequence<T>& ListSequence<T>::operator=(const ListSequence<T>& sequence)
{
    if (this == &sequence) {
        return *this;
    }

    LinkedList<T>* copiedItems = new LinkedList<T>(*sequence.items);
    delete this->items;
    this->items = copiedItems;
    return *this;
}

template<class T>
T& ListSequence<T>::operator[](size_t index)
{
    return (*this->items)[index]; 
}

template<class T>
const T& ListSequence<T>::operator[](size_t index) const
{
    return (*this->items)[index]; 
}

template<class T>
Sequence<T>* ListSequence<T>::operator+(Sequence<T>* other)
{
    if (other == nullptr) throw InvalidArgument("Cannot add null sequence");

    ListSequence<T>* result = this->CreateEmptyMut();

    try {
        for (size_t i = 0; i < this->GetLength(); ++i) {
            result->AppendInternal((*this)[i]);
        }
        for (size_t i = 0; i < other->GetLength(); ++i) {
            result->AppendInternal(other->Get(i));
        }
    }
    catch (...) {
        delete result;
        throw;
    }

    return result;
}

template<class T>
bool ListSequence<T>::operator==(Sequence<T>* other)
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
bool ListSequence<T>::operator!=(Sequence<T>* other)
{
    return !(*this == other);
}

#pragma endregion

template <class T>
Sequence<T>* ListSequence<T>::CreateEmpty() {
    return this->CreateEmptyMut();
}

template<class T>
IEnumerator<T>* ListSequence<T>::GetEnumerator()
{
    return new ListSequenceEnumerator<T>(this);
}

#pragma region IEnumerator

template<class T>
ListSequenceEnumerator<T>::ListSequenceEnumerator(ListSequence<T>* sequence)
{
    this->sequence = sequence;
    this->position = -1;
}

template<class T>
T ListSequenceEnumerator<T>::GetCurrent()
{
    if (position < 0 || position >= sequence->GetLength()) throw OutOfRange("Enumerator is out of bounds");

    return sequence->Get(position);
}

template<class T>
bool ListSequenceEnumerator<T>::MoveNext()
{
    if (position + 1 < sequence->GetLength()) {
        position++;
        return true;
    }

    return false;
}

template<class T>
void ListSequenceEnumerator<T>::Reset()
{
    this->position = -1;
}

#pragma endregion

#pragma region Mutable/Immutable

template<class T>
MutableListSequence<T>::MutableListSequence()
{
}

template<class T>
MutableListSequence<T>::MutableListSequence(T* items, size_t count)
{
    this->SetItems(items, count);
}

template<class T>
MutableListSequence<T>::MutableListSequence(const MutableListSequence<T>& sequence)
{
    this->CopyItems(sequence);
}

template<class T>
MutableListSequence<T>::MutableListSequence(std::initializer_list<T> items)
{
    this->SetItems(items);
}

template<class T>
MutableListSequence<T>& MutableListSequence<T>::operator=(const MutableListSequence<T>& sequence)
{
    this->ListSequence<T>::operator=(sequence);
    return *this;
}

template<class T>
ListSequence<T>* MutableListSequence<T>::Instance()
{
    return this;
}

template<class T>
ListSequence<T>* MutableListSequence<T>::CreateEmptyMut()
{
    return new MutableListSequence<T>();
}

template<class T>
ImmutableListSequence<T>::ImmutableListSequence()
{
}

template<class T>
ImmutableListSequence<T>::ImmutableListSequence(T* items, size_t count)
{
    this->SetItems(items, count);
}

template<class T>
ImmutableListSequence<T>::ImmutableListSequence(const ImmutableListSequence<T>& sequence)
{
    this->CopyItems(sequence);
}

template<class T>
ImmutableListSequence<T>::ImmutableListSequence(std::initializer_list<T> items)
{
    this->SetItems(items);
}

template<class T>
ImmutableListSequence<T>& ImmutableListSequence<T>::operator=(const ImmutableListSequence<T>& sequence)
{
    this->ListSequence<T>::operator=(sequence);
    return *this;
}

template<class T>
ListSequence<T>* ImmutableListSequence<T>::Instance()
{
    return new ImmutableListSequence<T>(*this);
}

template<class T>
ListSequence<T>* ImmutableListSequence<T>::CreateEmptyMut()
{
    return new ImmutableListSequence<T>();
}

#pragma endregion