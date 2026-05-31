#pragma once

#include "Interfaces.hpp"

template <class T>
class DefaultRing : public IRing<T> {
public:
    T Add(T a, T b) override { return a + b; }
    T Zero() override { return T{}; }
    T Inverse(T a) override { return -a; }
    T Multiply(T a, T b) override { return a * b; }
    T One() override { return T{1}; }
};