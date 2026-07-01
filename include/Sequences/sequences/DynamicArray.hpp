#pragma once

#include "../other/Exceptions.hpp"

template<class T>
class DynamicArray
{
    private:
        T* data;
        size_t size;

    public:
        DynamicArray();
        DynamicArray(T* items, size_t count);
        DynamicArray(size_t size);
        DynamicArray(const DynamicArray<T>& dynamicArray);
        DynamicArray(std::initializer_list<T> items);
        ~DynamicArray();

        DynamicArray<T>& operator=(const DynamicArray<T>& dynamicArray);
        T& operator[](size_t index);
        const T& operator[](size_t index) const;
        DynamicArray<T>* operator+(const DynamicArray<T>* other);
        bool operator==(const DynamicArray<T>* other);
        bool operator!=(const DynamicArray<T>* other);

        const T& Get(size_t index) const;
        size_t GetSize() const;

        void Set(size_t index, T value);
        void Resize(size_t newSize);
};

template<class T>
DynamicArray<T>::DynamicArray() : data(new T[0]), size(0)
{
}

template<class T>
DynamicArray<T>::DynamicArray(T* items, size_t count)
{
    if (count == 0) {
        this->data = new T[count];
        this->size = count;
        return;
    }

    if (items == nullptr) {
        throw InvalidArgument("DynamicArray::Constructor - Invalid Argument: Attempted to create an array with count = " + 
                              std::to_string(count) + ", but the 'items' pointer is null.");
    }

    this->data = new T[count];
    this->size = count;

    try {
        for (size_t i = 0; i < this->size; i++) {
            this->data[i] = items[i];
        }
    } catch (...) {
        delete[] this->data;
        throw; 
    }
}

template<class T>
DynamicArray<T>::DynamicArray(size_t size) : data(new T[size]), size(size)
{
}

template<class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& dynamicArray)
{
    this->size = dynamicArray.size;

    if (this->size == 0) {
        this->data = nullptr;
        return;
    }

    this->data = new T[this->size];

    try {
        for (size_t i = 0; i < this->size; i++) {
            this->data[i] = dynamicArray.data[i];
        }
    } catch (...) {
        delete[] this->data;
        throw; 
    }
}

template<class T>
DynamicArray<T>::DynamicArray(std::initializer_list<T> items)
{
    this->size = items.size();

    if (this->size == 0) {
        this->data = nullptr;
        return;
    }

    this->data = new T[this->size];

    size_t index = 0;
    try {
        for (const T& item : items) {
            this->data[index++] = item;
        }
    } catch (...) {
        delete[] this->data;
        throw; 
    }
}

template<class T>
DynamicArray<T>::~DynamicArray()
{
    delete[] this->data;
}

template<class T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& dynamicArray)
{
    if (this == &dynamicArray) {
        return *this;
    }

    T* newData = nullptr;

    if (dynamicArray.size > 0) {
        newData = new T[dynamicArray.size];
        try {
            for (size_t i = 0; i < dynamicArray.size; i++) {
                newData[i] = dynamicArray.data[i];
            }
        } catch (...) {
            delete[] newData;
            throw;
        }
    }

    delete[] this->data;

    this->data = newData;
    this->size = dynamicArray.size;

    return *this;
}

template<class T>
T& DynamicArray<T>::operator[](size_t index)
{
    if (index >= this->size) {
        throw OutOfRange("DynamicArray::operator[] - Out of Range: Attempted to access index " + std::to_string(index) + 
                         ", but the array size is only " + std::to_string(this->size) + ".");
    }

    return data[index];
}

template<class T>
const T& DynamicArray<T>::operator[](size_t index) const
{
    if (index >= this->size) {
        throw OutOfRange("DynamicArray::operator[] const - Out of Range: Attempted to access index " + std::to_string(index) + 
                         ", but the array size is only " + std::to_string(this->size) + ".");
    }
    
    return data[index];
}

template<class T>
DynamicArray<T>* DynamicArray<T>::operator+(const DynamicArray<T>* other)
{
    if (other == nullptr) {
        throw InvalidArgument("DynamicArray::operator+ - Invalid Argument: The right operand (other pointer) is null.");
    }

    size_t newSize = this->size + other->size;
    
    DynamicArray<T>* result = new DynamicArray<T>(newSize);

    try {
        for (size_t i = 0; i < this->size; ++i) {
            result->Set(i, this->data[i]);
        }

        for (size_t i = 0; i < other->size; ++i) {
            result->Set(this->size + i, other->data[i]);
        }
    }
    catch (...) {
        delete result;
        throw;
    }

    return result;
}

template<class T>
bool DynamicArray<T>::operator==(const DynamicArray<T>* other)
{
    if (other == nullptr) return false;

    if (this->size != other->size) return false;

    for (size_t i = 0; i < this->size; ++i) {
        if (this->data[i] != other->data[i]) return false;
    }

    return true;
}

template<class T>
bool DynamicArray<T>::operator!=(const DynamicArray<T>* other)
{
    return !(*this == other);
}

template<class T>
const T& DynamicArray<T>::Get(size_t index) const
{   
    if (index >= this->size) {
        throw OutOfRange("DynamicArray::Get - Out of Range: Attempted to read from index " + std::to_string(index) + 
                         ", but the array size is only " + std::to_string(this->size) + ".");
    }

    return this->data[index];
}

template<class T>
size_t DynamicArray<T>::GetSize() const
{
    return this->size;
}

template<class T>
void DynamicArray<T>::Set(size_t index, T value)
{
    if (index >= this->size) {
        throw OutOfRange("DynamicArray::Set - Out of Range: Attempted to write to index " + std::to_string(index) + 
                         ", but the array size is only " + std::to_string(this->size) + ".");
    }

    this->data[index] = value;
}

template<class T>
void DynamicArray<T>::Resize(size_t newSize)
{
    if (newSize == 0) {
        delete[] this->data;
        this->data = nullptr;
        this->size = 0;
        return;
    }

    T* newData = new T[newSize];

    size_t elementsToCopy = this->size < newSize ? this->size : newSize;

    try {
        for (size_t i = 0; i < elementsToCopy; i++) {
            newData[i] = this->data[i];
        }
    } catch (...) {
        delete[] newData;
        throw;
    }

    delete[] this->data;

    this->data = newData;
    this->size = newSize;
}
