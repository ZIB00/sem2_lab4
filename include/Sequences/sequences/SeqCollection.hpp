#pragma once

#include "../other/Exceptions.hpp"
#include "../other/ICollection.hpp"
#include "Sequence.hpp"
#include "../other/ICollection.hpp"

template< class T >
class SeqCollection : public ICollection<T>
{
private:
    Sequence<T>* _sequence;
    bool _isReadOnly;

    void ThrowReadOnly() {
        if( _isReadOnly ) throw LogicError("Collection is read-only");
    }

public:
    SeqCollection(Sequence<T>* Seq, bool readOnly = false) : _sequence(Seq), _isReadOnly(readOnly) {}

    IEnumerator<T>* GetEnumerator() override { return _sequence->GetEnumerator(); }

    size_t Count()      const override { return _sequence->GetLength(); }
    bool   IsReadOnly() const override { return _isReadOnly; }
    void   Add(T item) override { 
        ThrowReadOnly();
        _sequence = _sequence->Append(item); 
    }
    void Clear() override { 
        ThrowReadOnly();
        _sequence = _sequence->CreateEmpty(); 
    }
    bool Contains(T item) const override {
        IEnumerator<T>* enumerator = _sequence->GetEnumerator();
        while (enumerator->MoveNext()) {
            if (enumerator->Current() == item) {
                delete enumerator;
                return true;
            }
        }
        delete enumerator;
        return false;
    }
    void CopyTo(T* array, size_t arrayIndex = 0) const override {
        IEnumerator<T>* enumerator = _sequence->GetEnumerator();
        size_t i = arrayIndex;
        while (enumerator->MoveNext()) {
            array[i++] = enumerator->Current();
        }
        delete enumerator;
    }
    bool Remove(T item) override {
        ThrowReadOnly();
        
        size_t len = _sequence->GetLength();
        IEnumerator<T>* enumerator = _sequence->GetEnumerator();
        size_t i = 0;
        
        while (enumerator->MoveNext()) {
            if (enumerator->Current() == item) {
                Sequence<T>* newSeq = nullptr;
                
                if (len == 1) { newSeq = _sequence->CreateEmpty();
                } else if (i == 0) { newSeq = _sequence->GetSubsequence(1, len - 1);
                } else if (i == len - 1) { newSeq = _sequence->GetSubsequence(0, i - 1);
                } else {
                    Sequence<T>* firstPart = _sequence->GetSubsequence(0, i - 1);
                    Sequence<T>* secondPart = _sequence->GetSubsequence(i + 1, len - 1);
                    newSeq = firstPart->Concat(secondPart);
                }
                
                _sequence = newSeq;
                delete enumerator;
                return true;
            }
            ++i;
        }
        delete enumerator;
        return false;
    }
};