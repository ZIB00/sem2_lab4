#pragma once

#include "other/Exceptions.hpp"
#include <initializer_list>

template<class T>
class LinkedList
{
    private:
        struct Node
        {
            T value;
            Node* next;

            Node(T value, Node* next = nullptr);
        };

        Node* head;
        Node* tail;

    public:
        LinkedList(T* items, size_t count);
        LinkedList();
        LinkedList(const LinkedList<T>& list);
        LinkedList(std::initializer_list<T> items);
        ~LinkedList();

        LinkedList<T>& operator=(const LinkedList<T>& list);
        T& operator[](size_t index);
        const T& operator[](size_t index) const;
        LinkedList<T>* operator+(const LinkedList<T>* other);
        bool operator==(const LinkedList<T>* other);
        bool operator!=(const LinkedList<T>* other);

        T GetFirst();
        T GetLast();
        const T& Get(size_t index);
        void Set(size_t index, T value);
        LinkedList<T>* GetSubList(size_t startIndex, size_t endIndex);
        size_t GetLength();

        void Append(T item);
        void Prepend(T item);
        void InsertAt(T item, size_t index);
        LinkedList<T>* Concat(const LinkedList<T>* list);

        void Clear();
};

#include <string>

template<class T>
LinkedList<T>::Node::Node(T value, Node* next)
{
    this->value = value;
    this->next = next;
}

template<class T>
void LinkedList<T>::Clear()
{
    Node* current = this->head;

    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }

    this->head = nullptr;
    this->tail = nullptr;
}

template<class T>
LinkedList<T>::LinkedList(T* items, size_t count)
{
    if (count > 0 && items == nullptr) {
        throw InvalidArgument("LinkedList::Constructor - Invalid Argument: Attempted to create a list with count = " + 
                              std::to_string(count) + ", but the 'items' pointer is null.");
    }

    this->head = nullptr;
    this->tail = nullptr;

    try {
        for (size_t i = 0; i < count; i++) {
            this->Append(items[i]);
        }
    }
    catch (...) {
        this->Clear();
        throw;
    }
}

template<class T>
LinkedList<T>::LinkedList()
{
    this->head = nullptr;
    this->tail = nullptr;
}

template<class T>
LinkedList<T>::LinkedList(const LinkedList<T>& list)
{
    this->head = nullptr;
    this->tail = nullptr;

    Node* current = list.head;

    try {
        while (current != nullptr) {
            this->Append(current->value);
            current = current->next;
        }
    }
    catch (...) {
        this->Clear();
        throw;
    }
}

template<class T>
LinkedList<T>::LinkedList(std::initializer_list<T> items) : head(nullptr), tail(nullptr) 
{
    try {
        for (const T& item : items) 
        {
            this->Append(item);
        }
    }
    catch (...) {
        this->Clear(); 
        throw;
    }
}

template<class T>
LinkedList<T>::~LinkedList()
{
    this->Clear();
}

template<class T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& list)
{
    if (this != &list) {
        LinkedList<T> copy(list);

        Node* tempHead = this->head;
        Node* tempTail = this->tail;

        this->head = copy.head;
        this->tail = copy.tail;

        copy.head = tempHead;
        copy.tail = tempTail;
    }

    return *this;
}

template<class T>
T& LinkedList<T>::operator[](size_t index)
{
    Node* current = head;
    for (size_t i = 0; i < index; ++i) {
        if (current == nullptr) {
            throw OutOfRange("LinkedList::operator[] - Out of Range: Cannot access index " + std::to_string(index) + 
                             ". The list ended prematurely at index " + std::to_string(i) + ".");
        }
        current = current->next;
    }
    if (current == nullptr) {
        throw OutOfRange("LinkedList::operator[] - Out of Range: The list is empty or index " + std::to_string(index) + 
                         " is strictly greater than the maximum available index.");
    }
    
    return current->value;
}

template<class T>
const T& LinkedList<T>::operator[](size_t index) const
{
    Node* current = head;
    for (size_t i = 0; i < index; ++i) {
        if (current == nullptr) {
            throw OutOfRange("LinkedList::operator[] const - Out of Range: Cannot access index " + std::to_string(index) + 
                             ". The list ended prematurely at index " + std::to_string(i) + ".");
        }
        current = current->next;
    }
    if (current == nullptr) {
        throw OutOfRange("LinkedList::operator[] const - Out of Range: The list is empty or index " + std::to_string(index) + 
                         " is strictly greater than the maximum available index.");
    }
    
    return current->value;
}

template<class T>
LinkedList<T>* LinkedList<T>::operator+(const LinkedList<T>* other) {
    return this->Concat(other);
}

template<class T>
bool LinkedList<T>::operator==(const LinkedList<T>* other)
{
    if (other == nullptr) {
        return false;
    }
    if (this == other) {
        return true; 
    }

    Node* currentThis = this->head;
    Node* currentOther = other->head;

    while (currentThis != nullptr && currentOther != nullptr) {
        if (currentThis->value != currentOther->value) {
            return false;
        }
        
        currentThis = currentThis->next;
        currentOther = currentOther->next;
    }

    return currentThis == nullptr && currentOther == nullptr;
}

template<class T>
bool LinkedList<T>::operator!=(const LinkedList<T>* other)
{
    return !(*this == other);
}

template<class T>
T LinkedList<T>::GetFirst()
{
    if (this->head == nullptr) {
        throw OutOfRange("LinkedList::GetFirst - Out of Range: The list is completely empty (head is null).");
    }

    return this->head->value;
}

template<class T>
T LinkedList<T>::GetLast()
{
    if (this->tail == nullptr) {
        throw OutOfRange("LinkedList::GetLast - Out of Range: The list is completely empty (tail is null).");
    }

    return this->tail->value;
}

template<class T>
const T& LinkedList<T>::Get(size_t index)
{
    Node* current = this->head;
    size_t currentIndex = 0;

    while (current != nullptr) {
        if (currentIndex == index) {
            return current->value;
        }

        current = current->next;
        currentIndex++;
    }

    throw OutOfRange("LinkedList::Get - Out of Range: Index " + std::to_string(index) + 
                     " does not exist. Current list length is " + std::to_string(currentIndex) + ".");
}

template<class T>
void LinkedList<T>::Set(size_t index, T value)
{
    Node* current = this->head;
    size_t currentIndex = 0;

    while (current != nullptr) {
        if (currentIndex == index) {
            current->value = value;
        }

        current = current->next;
        currentIndex++;
    }

    throw OutOfRange("LinkedList::Get - Out of Range: Index " + std::to_string(index) + 
                     " does not exist. Current list length is " + std::to_string(currentIndex) + ".");
}

template<class T>
LinkedList<T>* LinkedList<T>::GetSubList(size_t startIndex, size_t endIndex)
{
    if (startIndex > endIndex) {
        throw InvalidArgument("LinkedList::GetSubList - Logical Error: startIndex (" + std::to_string(startIndex) + 
                              ") cannot be greater than endIndex (" + std::to_string(endIndex) + ").");
    }

    LinkedList<T>* result = new LinkedList<T>();
    Node* current = this->head;
    size_t currentIndex = 0;

    try {
        while (current != nullptr && currentIndex <= endIndex) {
            if (currentIndex >= startIndex) {
                result->Append(current->value);
            }

            current = current->next;
            currentIndex++;
        }

        if (endIndex >= currentIndex) {
            throw OutOfRange("LinkedList::GetSubList - Out of Range: endIndex (" + std::to_string(endIndex) + 
                             ") exceeds the actual list bounds. Max reached index was " + std::to_string(currentIndex - 1) + ".");
        }
    }
    catch (...) {
        delete result;
        throw;
    }

    return result;
}

template<class T>
size_t LinkedList<T>::GetLength()
{
    size_t length = 0;
    Node* current = this->head;

    while (current != nullptr) {
        length++;
        current = current->next;
    }

    return length;
}

template<class T>
void LinkedList<T>::Append(T item)
{
    Node* node = new Node(item);

    if (this->head == nullptr) {
        this->head = node;
        this->tail = node;
        return;
    }

    this->tail->next = node;
    this->tail = node;
}

template<class T>
void LinkedList<T>::Prepend(T item)
{
    Node* node = new Node(item, this->head);

    this->head = node;

    if (this->tail == nullptr) {
        this->tail = node;
    }
}

template<class T>
void LinkedList<T>::InsertAt(T item, size_t index)
{
    if (index < 0) {
        throw InvalidArgument("LinkedList::InsertAt - Invalid Argument: Index cannot be negative.");
    }

    if (index == 0) {
        this->Prepend(item);
        return;
    }

    Node* previous = this->head;
    size_t previousIndex = 0;

    while (previous != nullptr && previousIndex < index - 1) {
        previous = previous->next;
        previousIndex++;
    }

    if (previous == nullptr) {
        throw OutOfRange("LinkedList::InsertAt - Out of Range: Cannot insert item at index " + std::to_string(index) + 
                         ". The list only has elements up to index " + std::to_string(previousIndex) + ".");
    }

    Node* node = new Node(item, previous->next);
    previous->next = node;

    if (node->next == nullptr) {
        this->tail = node;
    }
}

template<class T>
LinkedList<T>* LinkedList<T>::Concat(const LinkedList<T>* list)
{
    if (list == nullptr) {
        throw InvalidArgument("LinkedList::Concat - Invalid Argument: The right operand (list pointer) is null.");
    }

    LinkedList<T>* result = new LinkedList<T>(*this);
    Node* current = list->head;

    try {
        while (current != nullptr) {
            result->Append(current->value);
            current = current->next;
        }
    }
    catch (...) {
        delete result;
        throw;
    }

    return result;
}