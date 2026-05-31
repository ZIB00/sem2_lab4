#pragma once

#include "DynamicArray.hpp"
#include "Sequence.hpp"
#include "other/Exceptions.hpp"
#include <algorithm>

template<class T>
class BitSequence : public Sequence<T>
{
    private:
        DynamicArray<unsigned char>* bytes;
        size_t bitCount;

        void SetBit(size_t index, bool value);

    public:
        BitSequence();
        BitSequence(T* items, size_t count);
        BitSequence(const BitSequence<T>& other);
        BitSequence(std::initializer_list<T> items);
        ~BitSequence() override;

        BitSequence<T>& operator=(const BitSequence<T>& other);
        Sequence<T>* operator+(Sequence<T>* other);
        bool operator==(Sequence<T>* other);
        bool operator!=(Sequence<T>* other);

        T GetFirst() override;
        T GetLast() override;
        T Get(size_t index) override;
        void Set(size_t index, T value) override;
        Sequence<T>* GetSubsequence(size_t startIndex, size_t endIndex) override;
        size_t GetLength() override;

        Sequence<T>* Append(T item) override;
        Sequence<T>* Prepend(T item) override;
        Sequence<T>* InsertAt(T item, size_t index) override;
        Sequence<T>* Concat(Sequence<T>* list) override;
        
        Sequence<T>* CreateEmpty() override;

        BitSequence<T>* NOT();
        BitSequence<T>* AND(const BitSequence<T>* other);
        BitSequence<T>* OR(const BitSequence<T>* other);
        BitSequence<T>* XOR(const BitSequence<T>* other);

        class Enumerator : public IEnumerator<T>
        {
            private:
                BitSequence<T>* sequence;
                size_t position;
                bool started;

            public:
                Enumerator(BitSequence<T>* seq);
                T GetCurrent() override;
                bool MoveNext() override;
                void Reset() override;
        };

        IEnumerator<T>* GetEnumerator() override;
};

#pragma region main_functions

#define BITS_IN_BYTE 8

template<class T>
void BitSequence<T>::SetBit(size_t index, bool value)
{
    size_t byteIndex = index / BITS_IN_BYTE;
    size_t bitOffset = index % BITS_IN_BYTE;

    unsigned char theByte = this->bytes->Get(byteIndex);

    if (value) {
        theByte |= (1 << bitOffset);
    } else {
        theByte &= ~(1 << bitOffset);
    }

    this->bytes->Set(byteIndex, theByte);
}

template<class T>
BitSequence<T>::BitSequence()
{
    this->bytes = new DynamicArray<unsigned char>(0);
    this->bitCount = 0;
}

template<class T>
BitSequence<T>::BitSequence(T* items, size_t count) : BitSequence()
{
    if (count < 0) throw InvalidArgument("Count cannot be negative");

    for (size_t i = 0; i < count; ++i) {
        this->Append(items[i]);
    }
}

template<class T>
BitSequence<T>::BitSequence(const BitSequence<T>& other)
{
    this->bytes = new DynamicArray<unsigned char>(*other.bytes);
    this->bitCount = other.bitCount;
}

template<class T>
BitSequence<T>::~BitSequence()
{
    delete this->bytes;
}

template<class T>
T BitSequence<T>::Get(size_t index)
{
    if (index >= this->bitCount) throw OutOfRange("Index out of bounds");

    size_t byteIndex = index / BITS_IN_BYTE;
    size_t bitOffset = index % BITS_IN_BYTE;

    unsigned char theByte = this->bytes->Get(byteIndex);

    return static_cast<T>((theByte >> bitOffset) & 1);
}

template<class T>
void BitSequence<T>::Set(size_t index, T value)
{
    if (index >= this->bitCount) throw OutOfRange("Index out of bounds");
    this->SetBit(index, static_cast<bool>(value));
}

template<class T>
T BitSequence<T>::GetFirst()
{
    if (this->bitCount == 0) throw OutOfRange("Sequence is empty");

    return this->Get(0);
}

template<class T>
T BitSequence<T>::GetLast()
{
    if (this->bitCount == 0) throw OutOfRange("Sequence is empty");

    return this->Get(this->bitCount - 1);
}

template<class T>
size_t BitSequence<T>::GetLength()
{
    return this->bitCount;
}

template<class T>
Sequence<T>* BitSequence<T>::GetSubsequence(size_t startIndex, size_t endIndex)
{
    if (startIndex < 0 || endIndex < 0 || startIndex > endIndex || endIndex >= this->bitCount) {
        throw OutOfRange("Invalid sequence range");
    }

    BitSequence<T>* result = new BitSequence<T>();
    for (size_t i = startIndex; i <= endIndex; ++i) {
        result->Append(this->Get(i));
    }
    return result;
}

template<class T>
Sequence<T>* BitSequence<T>::Append(T item)
{
    if (this->bitCount % BITS_IN_BYTE == 0) {
        size_t currentBytes = this->bytes->GetSize();
        this->bytes->Resize(currentBytes + 1);
        this->bytes->Set(currentBytes, 0);
    }

    this->SetBit(this->bitCount, static_cast<bool>(item));
    this->bitCount++;

    return this;
}

template<class T>
Sequence<T>* BitSequence<T>::InsertAt(T item, size_t index)
{
    if (index < 0) throw InvalidArgument("Index cannot be negative");
    if (index > this->bitCount) throw OutOfRange("Index is out of range");

    if (index == this->bitCount) {
        return this->Append(item);
    }

    this->Append(static_cast<T>(false)); 

    for (size_t i = this->bitCount - 1; i > index; i--) {
        bool previousBit = static_cast<bool>(this->Get(i - 1));
        this->SetBit(i, previousBit);
    }

    this->SetBit(index, static_cast<bool>(item));

    return this;
}

template<class T>
Sequence<T>* BitSequence<T>::Prepend(T item)
{
    return this->InsertAt(item, 0);
}

template<class T>
Sequence<T>* BitSequence<T>::Concat(Sequence<T>* list)
{
    if (list == nullptr) throw InvalidArgument("Sequence cannot be null");

    for (size_t i = 0; i < list->GetLength(); ++i) {
        this->Append(list->Get(i));
    }

    return this;
}

#pragma endregion

#pragma region operators (=, [], const [], +, ==, !=)

template<class T>
BitSequence<T>& BitSequence<T>::operator=(const BitSequence<T>& other)
{
    if (this == &other) {
        return *this;
    }

    DynamicArray<unsigned char>* newBytes = new DynamicArray<unsigned char>(*other.bytes);
    delete this->bytes;
    this->bytes = newBytes;
    this->bitCount = other.bitCount;

    return *this;
}

template<class T>
Sequence<T>* BitSequence<T>::operator+(Sequence<T>* other)
{
    if (other == nullptr) throw InvalidArgument("Cannot add null sequence");

    auto result = new BitSequence<T>();

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
bool BitSequence<T>::operator==(Sequence<T>* other)
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
bool BitSequence<T>::operator!=(Sequence<T>* other)
{
    return !(*this == other);
}

#pragma endregion

template <class T>
Sequence<T>* BitSequence<T>::CreateEmpty() {
    return new BitSequence<T>();
}

template<class T>
BitSequence<T>* BitSequence<T>::NOT()
{
    BitSequence<T>* result = new BitSequence<T>();
    size_t byteSize = this->bytes->GetSize();
    
    result->bytes->Resize(byteSize);
    result->bitCount = this->bitCount;

    for (size_t i = 0; i < byteSize; ++i) {
        result->bytes->Set(i, ~this->bytes->Get(i));
    }

    return result;
}

template<class T>
BitSequence<T>* BitSequence<T>::AND(const BitSequence<T>* other)
{
    if (other == nullptr) throw InvalidArgument("Other sequence cannot be null");

    BitSequence<T>* result = new BitSequence<T>();
    size_t maxBytes = std::max(this->bytes->GetSize(), other->bytes->GetSize());
    
    result->bytes->Resize(maxBytes);
    result->bitCount = std::max(this->bitCount, other->bitCount);

    for (size_t i = 0; i < maxBytes; ++i) {
        unsigned char byteA = 0;
        if (i < this->bytes->GetSize()) {
            byteA = this->bytes->Get(i);
        }

        unsigned char byteB = 0;
        if (i < other->bytes->GetSize()) {
            byteB = other->bytes->Get(i);
        }

        result->bytes->Set(i, byteA & byteB);
    }

    return result;
}

template<class T>
BitSequence<T>* BitSequence<T>::OR(const BitSequence<T>* other)
{
    if (other == nullptr) throw InvalidArgument("Other sequence cannot be null");

    BitSequence<T>* result = new BitSequence<T>();
    size_t maxBytes = std::max(this->bytes->GetSize(), other->bytes->GetSize());
    
    result->bytes->Resize(maxBytes);
    result->bitCount = std::max(this->bitCount, other->bitCount);

    for (size_t i = 0; i < maxBytes; ++i) {
        unsigned char byteA = 0;
        if (i < this->bytes->GetSize()) {
            byteA = this->bytes->Get(i);
        }

        unsigned char byteB = 0;
        if (i < other->bytes->GetSize()) {
            byteB = other->bytes->Get(i);
        }

        result->bytes->Set(i, byteA | byteB);
    }

    return result;
}

template<class T>
BitSequence<T>* BitSequence<T>::XOR(const BitSequence<T>* other)
{
    if (other == nullptr) throw InvalidArgument("Other sequence cannot be null");

    BitSequence<T>* result = new BitSequence<T>();
    size_t maxBytes = std::max(this->bytes->GetSize(), other->bytes->GetSize());
    
    result->bytes->Resize(maxBytes);
    result->bitCount = std::max(this->bitCount, other->bitCount);

    for (size_t i = 0; i < maxBytes; ++i) {
        unsigned char byteA = 0;
        if (i < this->bytes->GetSize()) {
            byteA = this->bytes->Get(i);
        }

        unsigned char byteB = 0;
        if (i < other->bytes->GetSize()) {
            byteB = other->bytes->Get(i);
        }

        result->bytes->Set(i, byteA ^ byteB);
    }

    return result;
}

#pragma region IEnumerator

template<class T>
BitSequence<T>::Enumerator::Enumerator(BitSequence<T>* seq) 
    : sequence(seq), position(0), started(false) 
{
}

template<class T>
T BitSequence<T>::Enumerator::GetCurrent()
{
    if (!started || position >= sequence->GetLength()) {
        throw OutOfRange("BitSequence::Enumerator - Out of bounds");
    }
    return sequence->Get(position);
}

template<class T>
bool BitSequence<T>::Enumerator::MoveNext()
{
    if (!started) {
        started = true;
        position = 0;
    } else {
        position++;
    }
    return position < sequence->GetLength();
}

template<class T>
void BitSequence<T>::Enumerator::Reset()
{
    started = false;
    position = 0;
}

template<class T>
IEnumerator<T>* BitSequence<T>::GetEnumerator()
{
    return new Enumerator(this);
}

#pragma endregion