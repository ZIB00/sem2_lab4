#pragma once

#include "IEnumerable.hpp"
#include <cstddef>

template<class T>
class ICollection : public IEnumerable<T>
{
public:
    virtual ~ICollection() = default;

    //virtual IEnumerator<T> GetEnumerator() = 0;

    virtual size_t Count()          const = 0;
    virtual bool   IsReadOnly()     const = 0;
    virtual void   Add(T item)            = 0;
    virtual void   Clear()                = 0;
    virtual bool   Contains(T item) const = 0;
    virtual void   CopyTo(T* array, size_t arrayIndex) const = 0;
    virtual bool   Remove(T item)         = 0;
};