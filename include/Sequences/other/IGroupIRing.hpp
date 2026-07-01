#pragma once

template< class T >
class ISemiGroup //Для любого a, b —> (a o b) o c = a o (b o c)
{
public:
    virtual ~ISemiGroup() = default;

    virtual T Add(T left, T right) = 0;
};

template< class T >
class IMonoid : public ISemiGroup<T> 
{
public:
    virtual ~IMonoid() = default;

    virtual T Zero() = 0; //Для любого a Сущ. e т.ч. a o e = e o a = a
};

template <class T>
class IGroup : public IMonoid<T> 
{
public:
    virtual ~IGroup() = default;
    
    virtual T Inverse(T item) = 0; //Для любого a сущ. h = a^-1 т.ч. a o h = h o a = e
};

//Для любого a, b в G —> a o b = b o a — Абелева группа 
//Кольцо (коммутативное) — это множество R с двумя бинарными операциями + и x такое, что:
//(R, +) — абелева группа;
//(R, x) — (коммутативный) моноид;
//x дистрибутивно относительно + — Для любого a, b, c в R : (a + b) x c = a x c + b x c (при чём как слева, так и справа).
template <class T>
class IRing : public IGroup<T>  
{
public:
    virtual ~IRing() = default;

    virtual T One() = 0;
    virtual T Times(T left, T right) = 0;
};

template <class T>
class DefaultRing : public IRing<T> {
public:
    T Add(T left, T right)      override { return left + right; }
    T Zero()                    override { return T{};}
    T Inverse(T item)           override { return -item; }
    T Times(T left, T right)    override { return left * right; }
    T One()                     override { return T{1}; }
};