#pragma once

#include <utility>
#include "Exceptions.hpp"

//std::nullopt
struct nullopt_t {
    explicit constexpr nullopt_t(int) {}
};

inline constexpr nullopt_t nullopt{0};

template<typename T>
class Option {
private:
    alignas(T) unsigned char storage[sizeof(T)];
    bool hasValue;

    T* ptr() { return reinterpret_cast<T*>(storage); }
    const T* ptr() const { return reinterpret_cast<const T*>(storage); }

    void destroy() {
        if (hasValue) {
            ptr()->~T(); 
            hasValue = false;
        } 
    }

public:
    using valueType = T;

    Option() noexcept : hasValue(false) {}
    Option(nullopt_t) noexcept : hasValue(false) {}

    template<typename U = T, typename = std::enable_if_t<std::is_constructible_v<T, U&&>>>
    Option(U&& value) : hasValue(false) {
        
        new (ptr()) T(std::forward<U>(value));

        hasValue = true;

    } 

    Option(const Option& other) noexcept : hasValue(false) {
        if (other.hasValue) {
            new (ptr()) T(*other.ptr()); 
            hasValue = true;
        }
    }

    Option(Option&& other) noexcept : hasValue(false) {
        if (other.hasValue) {
            new (ptr()) T(std::move(*other.ptr()));
            hasValue = true;
        }
    }

    ~Option() { destroy(); }

    Option& operator=(nullopt_t) noexcept {
        destroy();
        return *this;
    }

    Option& operator=(const Option& other) {
        if (this == &other) return *this;
        if (hasValue && other.hasValue) {
            *ptr() = *other.ptr();
        } else if (other.hasValue) {
            new (ptr()) T(*other.ptr());
            hasValue = true;
        } else {
            destroy();
        }
        return *this;
    }

    bool HasValue() const noexcept { return hasValue; }

    T& operator*() & { return *ptr(); }
    const T& operator*() const & { return *ptr(); }
    T&& operator*() && { return std::move(*ptr()); }

    T* operator->() { return ptr(); }
    const T* operator->() const { return ptr(); }

    T& Value() & {
        if (!hasValue) throw BadOptionalAccess();
        return *ptr();
    }
    const T& Value() const & {
        if (!hasValue) throw BadOptionalAccess();
        return *ptr();
    }
    T&& Value() && {
        if (!hasValue) throw BadOptionalAccess();
        return std::move(*ptr());
    }

    template<typename U>
    T ValueOr(U&& default_value) const & {
        return hasValue ? *ptr() : static_cast<T>(std::forward<U>(default_value));
    }
    template<typename U>
    T ValueOr(U&& default_value) && {
        return hasValue ? std::move(*ptr()) : static_cast<T>(std::forward<U>(default_value));
    }

    void Reset() noexcept { destroy(); }
};