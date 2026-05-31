#pragma once

#include "ArraySequence.hpp"
#include "ListSequence.hpp"

template<class T>
class AdaptiveSequence : public Sequence<T>
{
    private:
        Sequence<T>* sequence;
        size_t getCount;
        size_t insertCount;

        void SwitchToArraySequence();
        void SwitchToListSequence();
        void UpdateStrategy();
        void CopyFrom(const AdaptiveSequence<T>& other);

    public:
        AdaptiveSequence();
        AdaptiveSequence(T* items, size_t count);
        AdaptiveSequence(const AdaptiveSequence<T>& other);
        AdaptiveSequence(std::initializer_list<T> items);
        ~AdaptiveSequence() override;

        AdaptiveSequence<T>& operator=(const AdaptiveSequence<T>& other);
        T& operator[](size_t index);
        const T& operator[](size_t index) const;
        Sequence<T>* operator+(Sequence<T>* other);
        bool operator==(Sequence<T>* other);
        bool operator!=(Sequence<T>* other);

        T GetFirst() override;
        T GetLast() override;
        const T& Get(size_t index);
        void Set( size_t index, T value);
        Sequence<T>* GetSubsequence(size_t startIndex, size_t endIndex) override;
        size_t GetLength() override;
        Sequence<T>* Append(T item) override;
        Sequence<T>* Prepend(T item) override;
        Sequence<T>* InsertAt(T item, size_t index) override;
        Sequence<T>* Concat(Sequence<T>* list) override;

        Sequence<T>* CreateEmpty() override;

        IEnumerator<T>* GetEnumerator() override;
};

#pragma region sequence main functions

template<class T>
AdaptiveSequence<T>::AdaptiveSequence()
{
    this->sequence = new MutableArraySequence<T>();
    this->getCount = 0;
    this->insertCount = 0;
}

template<class T>
AdaptiveSequence<T>::AdaptiveSequence(T* items, size_t count)
{
    this->sequence = new MutableArraySequence<T>(items, count);
    this->getCount = 0;
    this->insertCount = 0;
}

template<class T>
AdaptiveSequence<T>::AdaptiveSequence(const AdaptiveSequence<T>& other)
{
    this->sequence = nullptr;
    this->CopyFrom(other);
}

template<class T>
AdaptiveSequence<T>::AdaptiveSequence(std::initializer_list<T> items)
{
    this->sequence = new MutableArraySequence<T>(items);
    
    this->getCount = 0;
    this->insertCount = 0;
}

template<class T>
AdaptiveSequence<T>::~AdaptiveSequence()
{
    delete this->sequence;
}

template<class T>
void AdaptiveSequence<T>::CopyFrom(const AdaptiveSequence<T>& other)
{
    Sequence<T>* copiedSequence = nullptr;

    if (dynamic_cast<MutableListSequence<T>*>(other.sequence) != nullptr) {
        copiedSequence = new MutableListSequence<T>();
    }
    else {
        copiedSequence = new MutableArraySequence<T>();
    }

    try {
        for (size_t index = 0; index < other.sequence->GetLength(); ++index) {
            copiedSequence->Append(other.sequence->Get(index));
        }
    }
    catch (...) {
        delete copiedSequence;
        throw;
    }

    delete this->sequence;
    this->sequence = copiedSequence;
    this->getCount = other.getCount;
    this->insertCount = other.insertCount;
}

template<class T>
void AdaptiveSequence<T>::SwitchToArraySequence()
{
    if (dynamic_cast<MutableArraySequence<T>*>(this->sequence) != nullptr) {
        return;
    }

    Sequence<T>* newSequence = new MutableArraySequence<T>();

    try {
        for (size_t index = 0; index < this->sequence->GetLength(); ++index) {
            newSequence->Append(this->sequence->Get(index));
        }
    }
    catch (...) {
        delete newSequence;
        throw;
    }

    delete this->sequence;
    this->sequence = newSequence;
}

template<class T>
void AdaptiveSequence<T>::SwitchToListSequence()
{
    if (dynamic_cast<MutableListSequence<T>*>(this->sequence) != nullptr) {
        return;
    }

    Sequence<T>* newSequence = new MutableListSequence<T>();

    try {
        for (size_t index = 0; index < this->sequence->GetLength(); ++index) {
            newSequence->Append(this->sequence->Get(index));
        }
    }
    catch (...) {
        delete newSequence;
        throw;
    }

    delete this->sequence;
    this->sequence = newSequence;
}

template<class T>
void AdaptiveSequence<T>::UpdateStrategy()
{
    if (this->insertCount > this->getCount + 5) {
        this->SwitchToListSequence();
    }
    else if (this->getCount > this->insertCount + 5) {
        this->SwitchToArraySequence();
    }
}

template<class T>
T AdaptiveSequence<T>::GetFirst()
{
    this->getCount++;
    this->UpdateStrategy();
    return this->sequence->GetFirst();
}

template<class T>
T AdaptiveSequence<T>::GetLast()
{
    this->getCount++;
    this->UpdateStrategy();
    return this->sequence->GetLast();
}

template<class T>
const T& AdaptiveSequence<T>::Get(size_t index)
{
    this->getCount++;
    this->UpdateStrategy();
    return this->sequence->Get(index);
}

template<class T>
void AdaptiveSequence<T>::Set(size_t index, T value)
{
    this->getCount++;
    this->UpdateStrategy();
    this->sequence->Set( index, value );
}

template<class T>
Sequence<T>* AdaptiveSequence<T>::GetSubsequence(size_t startIndex, size_t endIndex)
{
    if (startIndex < 0) throw InvalidArgument("Start index cannot be negative");
    if (endIndex < 0) throw InvalidArgument("End index cannot be negative");
    if (startIndex > endIndex) throw InvalidArgument("Start index cannot be greater than end index");
    if (endIndex >= this->GetLength()) throw OutOfRange("Index is out of range");

    AdaptiveSequence<T>* result = new AdaptiveSequence<T>();

    if (dynamic_cast<MutableListSequence<T>*>(this->sequence) != nullptr) {
        result->SwitchToListSequence();
    }

    try {
        for (size_t index = startIndex; index <= endIndex; ++index) {
            result->sequence->Append(this->sequence->Get(index));
        }
    }
    catch (...) {
        delete result;
        throw;
    }

    return result;
}

template<class T>
size_t AdaptiveSequence<T>::GetLength()
{
    return this->sequence->GetLength();
}

template<class T>
Sequence<T>* AdaptiveSequence<T>::Append(T item)
{
    this->insertCount++;
    this->UpdateStrategy();
    this->sequence->Append(item);
    return this;
}

template<class T>
Sequence<T>* AdaptiveSequence<T>::Prepend(T item)
{
    this->insertCount++;
    this->UpdateStrategy();
    this->sequence->Prepend(item);
    return this;
}

template<class T>
Sequence<T>* AdaptiveSequence<T>::InsertAt(T item, size_t index)
{
    this->insertCount++;
    this->UpdateStrategy();
    this->sequence->InsertAt(item, index);
    return this;
}

template<class T>
Sequence<T>* AdaptiveSequence<T>::Concat(Sequence<T>* list)
{
    this->insertCount += list == nullptr ? 0 : list->GetLength();
    this->UpdateStrategy();
    this->sequence->Concat(list);
    return this;
}

#pragma endregion

template <class T>
Sequence<T>* AdaptiveSequence<T>::CreateEmpty() {
    return new AdaptiveSequence<T>();
}

#pragma region operators (=, [], const [], +, ==, !=)

template<class T>
AdaptiveSequence<T>& AdaptiveSequence<T>::operator=(const AdaptiveSequence<T>& other)
{
    if (this == &other) {
        return *this;
    }

    this->CopyFrom(other);
    return *this;
}

template<class T>
T& AdaptiveSequence<T>::operator[](size_t index)
{
    return (*this->sequence)[index]; 
}

template<class T>
const T& AdaptiveSequence<T>::operator[](size_t index) const
{
    return (*this->sequence)[index]; 
}

template<class T>
Sequence<T>* AdaptiveSequence<T>::operator+(Sequence<T>* other)
{
    if (other == nullptr) throw InvalidArgument("Cannot add null sequence");

    auto result = new AdaptiveSequence<T>();

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
bool AdaptiveSequence<T>::operator==(Sequence<T>* other)
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
bool AdaptiveSequence<T>::operator!=(Sequence<T>* other)
{
    return !(*this == other);
}

#pragma endregion

template<class T>
IEnumerator<T>* AdaptiveSequence<T>::GetEnumerator()
{
    return this->sequence->GetEnumerator();
}