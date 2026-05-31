#pragma once

template <class T>
class IGroup {
public:
    virtual ~IGroup() = default;
    
    virtual T Add(T a, T b) = 0;
    virtual T Zero() = 0;
    virtual T Inverse(T a) = 0;
};

template <class T>
class IRing : public IGroup<T> {
public:
    virtual T Multiply(T a, T b) = 0;
    virtual T One() = 0;
};