#pragma once

#include "DynamicArray.hpp"
#include "LinkedList.hpp"
#include "Sequence.hpp"

constexpr size_t SEGMENT_SIZE = 10;

template<class T>
class SegmentedList : public Sequence<T>
{
    private:
        LinkedList<DynamicArray<T>*>* segments;

        size_t length;

        void Clear();
        void ValidateNotEmpty();
        DynamicArray<T>* FindNode(size_t index, size_t& localIndex);
        void Set(size_t index, T value);

    public:
        SegmentedList();
        SegmentedList(T* items, size_t count);
        SegmentedList(const SegmentedList<T>& list);
        SegmentedList(std::initializer_list<T> items);
        ~SegmentedList() override;

        SegmentedList<T>& operator=(const SegmentedList<T>& list);
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

        class Enumerator : public IEnumerator<T>
        {
            private:
                SegmentedList<T>* list;
                size_t segmentIndex;
                size_t localIndex;
                bool started;

            public:
                Enumerator(SegmentedList<T>* list);
                T GetCurrent() override;
                bool MoveNext() override;
                void Reset() override;
        };

        IEnumerator<T>* GetEnumerator() override;
};

#pragma once

#pragma region sequence main functions

template<class T>
SegmentedList<T>::SegmentedList()
{
    this->segments = new LinkedList<DynamicArray<T>*>();
    this->length = 0;
}

template<class T>
SegmentedList<T>::SegmentedList(T* items, size_t count)
{
    this->segments = new LinkedList<DynamicArray<T>*>();
    this->length = 0;

    if (count > 0 && items == nullptr) {
        throw InvalidArgument("SegmentedList::Constructor - Invalid Argument: Items array cannot be null when count > 0.");
    }

    try {
        for (size_t i = 0; i < count; ++i) {
            this->Append(items[i]);
        }
    }
    catch (...) {
        this->Clear();
        delete this->segments;
        throw;
    }
}

template<class T>
SegmentedList<T>::SegmentedList(const SegmentedList<T>& list)
{
    this->segments = new LinkedList<DynamicArray<T>*>();
    this->length = 0;

    try {
        for (size_t i = 0; i < list.length; ++i) {
            this->Append(list[i]);
        }
    }
    catch (...) {
        this->Clear();
        delete this->segments;
        throw;
    }
}

template<class T>
SegmentedList<T>::SegmentedList(std::initializer_list<T> items) 
    : SegmentedList(const_cast<T*>(items.begin()), items.size())
{

}

template<class T>
SegmentedList<T>::~SegmentedList()
{
    this->Clear();
    delete this->segments;
}

template<class T>
void SegmentedList<T>::Clear()
{
    if (this->segments != nullptr) {
        for (size_t i = 0; i < this->segments->GetLength(); ++i) {
            delete this->segments->Get(i);
        }
        this->segments->Clear(); 
    }
    this->length = 0;
}

template<class T>
void SegmentedList<T>::ValidateNotEmpty()
{
    if (this->length == 0) {
        throw OutOfRange("SegmentedList::ValidateNotEmpty - Out of Range: The segmented list is empty.");
    }
}

template<class T>
DynamicArray<T>* SegmentedList<T>::FindNode(size_t index, size_t& localIndex)
{
    if (index >= this->length) {
        throw OutOfRange("SegmentedList::FindNode - Out of Range: Attempted to access index " + 
                         std::to_string(index) + ", but the list length is only " + std::to_string(this->length) + ".");
    }

    size_t remaining = index;
    
    // Проходим по списку сегментов и ищем тот, в который попадает глобальный индекс
    for (size_t i = 0; i < this->segments->GetLength(); ++i) {
        DynamicArray<T>* currentSegment = this->segments->Get(i);
        size_t currentSegmentSize = currentSegment->GetSize();

        if (remaining < currentSegmentSize) {
            localIndex = remaining;
            return currentSegment;
        }
        remaining -= currentSegmentSize;
    }

    throw OutOfRange("SegmentedList::FindNode - Out of Range: Index not found in any segment.");
}

template<class T>
void SegmentedList<T>::Set(size_t index, T value)
{
    size_t localIndex = 0;
    DynamicArray<T>* targetSegment = this->FindNode(index, localIndex);
    targetSegment->Set(localIndex, value);
}

template<class T>
T SegmentedList<T>::GetFirst()
{
    this->ValidateNotEmpty();
    return this->segments->Get(0)->Get(0);
}

template<class T>
T SegmentedList<T>::GetLast()
{
    this->ValidateNotEmpty();
    DynamicArray<T>* lastSegment = this->segments->Get(this->segments->GetLength() - 1);
    return lastSegment->Get(lastSegment->GetSize() - 1);
}

template<class T>
const T& SegmentedList<T>::Get(size_t index)
{
    size_t localIndex = 0;
    DynamicArray<T>* targetSegment = this->FindNode(index, localIndex);
    return targetSegment->Get(localIndex);
}

template<class T>
void SegmentedList<T>::Set(size_t index, T value)
{
    size_t localIndex = 0;
    DynamicArray<T>* targetSegment = this->FindNode(index, localIndex);
    targetSegment->Set(localIndex, T value);
}

template<class T>
Sequence<T>* SegmentedList<T>::GetSubsequence(size_t startIndex, size_t endIndex)
{
    if (startIndex > endIndex) throw InvalidArgument("SegmentedList::GetSubsequence - Invalid Argument: Start index cannot be greater than end index.");
    if (endIndex >= this->length) throw OutOfRange("SegmentedList::GetSubsequence - Out of Range: End index is out of bounds.");

    SegmentedList<T>* result = new SegmentedList<T>();

    try {
        for (size_t i = startIndex; i <= endIndex; ++i) {
            result->Append(this->Get(i));
        }
    }
    catch (...) {
        delete result;
        throw;
    }

    return result;
}

template<class T>
size_t SegmentedList<T>::GetLength()
{
    return this->length;
}

template<class T>
Sequence<T>* SegmentedList<T>::Append(T item)
{
    if (this->segments->GetLength() == 0 || this->segments->Get(this->segments->GetLength() - 1)->GetSize() == SEGMENT_SIZE) {
        DynamicArray<T>* newSegment = new DynamicArray<T>(1);
        newSegment->Set(0, item);
        this->segments->Append(newSegment);
    }
    else {
        DynamicArray<T>* lastSegment = this->segments->Get(this->segments->GetLength() - 1);
        size_t newSize = lastSegment->GetSize() + 1;
        
        lastSegment->Resize(newSize);
        lastSegment->Set(newSize - 1, item);
    }

    this->length++;
    return this;
}

template<class T>
Sequence<T>* SegmentedList<T>::Prepend(T item)
{
    if (this->segments->GetLength() == 0 || this->segments->Get(0)->GetSize() == SEGMENT_SIZE) {
        DynamicArray<T>* newSegment = new DynamicArray<T>(1);
        newSegment->Set(0, item);
        this->segments->Prepend(newSegment);
    }
    else {
        DynamicArray<T>* firstSegment = this->segments->Get(0);
        size_t newSize = firstSegment->GetSize() + 1;
        
        firstSegment->Resize(newSize);
        
        for (size_t i = newSize - 1; i > 0; --i) {
            firstSegment->Set(i, firstSegment->Get(i - 1));
        }
        firstSegment->Set(0, item);
    }

    this->length++;
    return this;
}

template<class T>
Sequence<T>* SegmentedList<T>::InsertAt(T item, size_t index)
{
    if (index > this->length) {
        throw OutOfRange("SegmentedList::InsertAt - Out of Range: Attempted to insert at index " + 
                         std::to_string(index) + " but length is " + std::to_string(this->length) + ".");
    }

    if (index == 0) return this->Prepend(item);
    if (index == this->length) return this->Append(item);

    size_t oldLength = this->length;
    this->Append(item);

    for (size_t i = oldLength; i > index; --i) {
        this->Set(i, this->Get(i - 1));
    }

    this->Set(index, item);
    return this;
}

template<class T>
Sequence<T>* SegmentedList<T>::Concat(Sequence<T>* list)
{
    if (list == nullptr) throw InvalidArgument("SegmentedList::Concat - Invalid Argument: Target list is null.");
    
    for (size_t i = 0; i < list->GetLength(); ++i) {
        this->Append(list->Get(i));
    }

    return this;
}

template <class T>
Sequence<T>* SegmentedList<T>::CreateEmpty() {
    return new SegmentedList<T>();
}

#pragma endregion

#pragma region operators (=, [], const [], +, ==, !=)

template<class T>
SegmentedList<T>& SegmentedList<T>::operator=(const SegmentedList<T>& list)
{
    if (this == &list) return *this;

    this->Clear();

    for (size_t i = 0; i < list.length; ++i) {
        this->Append(list[i]);
    }

    return *this;
}

template<class T>
T& SegmentedList<T>::operator[](size_t index)
{
    size_t localIndex = 0;
    DynamicArray<T>* targetSegment = this->FindNode(index, localIndex);
    return (*targetSegment)[localIndex];
}

template<class T>
const T& SegmentedList<T>::operator[](size_t index) const
{
    size_t localIndex = 0;
    DynamicArray<T>* targetSegment = const_cast<SegmentedList<T>*>(this)->FindNode(index, localIndex);
    return (*targetSegment)[localIndex];
}

template<class T>
Sequence<T>* SegmentedList<T>::operator+(Sequence<T>* other)
{
    if (other == nullptr) throw InvalidArgument("SegmentedList::operator+ - Invalid Argument: Right operand is null.");
    
    SegmentedList<T>* result = new SegmentedList<T>();

    try {
        for (size_t i = 0; i < this->GetLength(); ++i) {
            result->Append((*this)[i]);
        }
        for (size_t i = 0; i < other->GetLength(); ++i) {
            result->Append(other->Get(i));
        }
    }
    catch (...) {
        delete result;
        throw;
    }

    return result;
}

template<class T>
bool SegmentedList<T>::operator==(Sequence<T>* other)
{
    if (this == other) return true;
    if (other == nullptr || this->GetLength() != other->GetLength()) return false;
    
    for (size_t i = 0; i < this->GetLength(); ++i) {
        if (this->Get(i) != other->Get(i)) return false;
    }
    return true;
}

template<class T>
bool SegmentedList<T>::operator!=(Sequence<T>* other)
{
    return !(*this == other);
}

#pragma endregion

#pragma region IEnumerator

template<class T>
SegmentedList<T>::Enumerator::Enumerator(SegmentedList<T>* list)
    : list(list), segmentIndex(0), localIndex(0), started(false) 
{
}

template<class T>
T SegmentedList<T>::Enumerator::GetCurrent()
{
    if (!started || segmentIndex >= list->segments->GetLength()) {
        throw OutOfRange("SegmentedList::Enumerator - Out of bounds");
    }
    DynamicArray<T>* currentSegment = list->segments->Get(segmentIndex);
    return currentSegment->Get(localIndex);
}

template<class T>
bool SegmentedList<T>::Enumerator::MoveNext()
{
    if (!started) {
        started = true;
        segmentIndex = 0;
        localIndex = 0;
        
        while (segmentIndex < list->segments->GetLength()) {
            if (list->segments->Get(segmentIndex)->GetSize() > 0) return true;
            segmentIndex++;
        }
        return false;
    }

    if (segmentIndex >= list->segments->GetLength()) return false;

    localIndex++;
    
    if (localIndex < list->segments->Get(segmentIndex)->GetSize()) {
        return true;
    }

    localIndex = 0;
    segmentIndex++;
    while (segmentIndex < list->segments->GetLength()) {
        if (list->segments->Get(segmentIndex)->GetSize() > 0) return true;
        segmentIndex++;
    }

    return false;
}

template<class T>
void SegmentedList<T>::Enumerator::Reset()
{
    started = false;
    segmentIndex = 0;
    localIndex = 0;
}

template<class T>
IEnumerator<T>* SegmentedList<T>::GetEnumerator()
{
    return new Enumerator(this);
}

#pragma endregion