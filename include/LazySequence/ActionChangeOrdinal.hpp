#pragma once

#include <cstddef>

enum Action 
{
        INSERT,
        SET,
        REMOVE
};

template<class T>
struct Change 
{
        size_t targetIndex;
        T value;
        Action action;

        bool operator<(const Change<T>& other) const {
                return this->targetIndex < other.targetIndex;
        }
};

class Ordinal 
{
private:
    size_t infinity;
    size_t finiteness;

public:
    Ordinal( size_t fin, size_t inf = 0 ) : infinity(inf), finiteness(fin) {}
    static Ordinal Infinite( size_t inf ) { return Ordinal(0, inf); }
    static Ordinal Infinite() { return Ordinal(0, 1); }

    Ordinal operator+( const Ordinal& other ) const { 
        size_t newInfinity = this->infinity + other.infinity;
        size_t newfiniteness = ( other.infinity == 0) ? this->finiteness + other.finiteness : other.finiteness;

        return Ordinal( newfiniteness, newInfinity );
    }

    Ordinal operator-( size_t n ) const {
    return Ordinal( (this->finiteness >= n) ? this->finiteness - n : 0, this->infinity );
    }

    bool operator==( const Ordinal& other ) const { 
        return ( this->infinity == other.infinity && this->finiteness == other.finiteness) ;
    }

    bool operator>( const Ordinal& other ) const { 
        if( this->infinity != other.infinity ) return this->infinity > other.infinity;

        return this->finiteness > other.finiteness;
    }

    bool operator<( const Ordinal& other )  const { return other > *this; }
    bool operator>=( const Ordinal& other ) const { return !(*this < other); }
    bool operator<=( const Ordinal& other ) const { return !(*this > other); }

    bool IsInfinite()    const { return infinity != 0; }
    size_t GetInfinity() const { return infinity;}
    size_t GetSize()     const { return finiteness; }
};