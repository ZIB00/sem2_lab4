#pragma once

#include "Exceptions.hpp"

template <typename T>
class Option 
{
private:
    bool hasValue;
    union {
        T value;
    };

public:
    Option() : hasValue(false) {}
    Option(const T& val) : hasValue(true), value(val) {}
    Option(T&& val) : hasValue(true), value(std::move(val)) {}
    ~Option() {
        if (hasValue) {
            value.~T();
        }
    }

    Option<T>& operator=( const Option<T>& opt ) {
        this->hasValue = opt.hasValue;
        this->value = opt.value;
    }

    Option<T>& operator=( Option<T>&& opt ) {
        this->hasValue = std::move(opt.hasValue);
        this->value = std::move(opt.value);
    }
    
    bool HasValue() const {
        return hasValue;
    }
    explicit operator bool() const {
        return hasValue;
    }
    const T& Value() const {
        if (!hasValue) {
            throw BadOptionalAccess();
        }
        return value;
    }

    T ValueOr(const T& defaultValue) const {
        return hasValue ? value : defaultValue;
    }
};