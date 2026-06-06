#pragma once

enum Action {
        INSERT,
        SET,
        REMOVE
};

template<class T>
struct Change {
        size_t targetIndex;
        T value;
        Action action;

        bool operator<(const Change<T>& other) const {
                return this->targetIndex < other.targetIndex;
        }
};

class Cardinal {
    private:

    bool isInfinite; 
    size_t size; 

    Cardinal(bool isInfinite, size_t val) : isInfinite(isInfinite), size(val) {}

    public:

    Cardinal(size_t size) : Cardinal(false, size) {}
    static Cardinal Infinite() { return Cardinal(true, 0); }

    Cardinal operator+(size_t size) const { 
        return isInfinite ? Cardinal::Infinite() : Cardinal(false, this->size + size); 
        }

    Cardinal operator-(size_t size) const { 
        if(isInfinite) return Cardinal::Infinite();
        return Cardinal(false, (this->size >= size) ? this->size - size : 0); 
        }

    bool IsInfinite() const { return isInfinite; }
    size_t GetSize() const { return size; }
};