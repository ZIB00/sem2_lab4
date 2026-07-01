#pragma once

#include "Exceptions.hpp"

template<class T>
class IEnumerator
{
    public:
        virtual ~IEnumerator() = default;

        virtual T    Current()  = 0;
        virtual bool MoveNext() = 0;
        virtual void Reset()    = 0;
};