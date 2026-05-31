#pragma once

#include <functional>

#include "../sequences/Sequence.hpp"
#include "Option.hpp"
#include "IEnumerator.hpp"

namespace SequenceUtils {

    template<class T, class T2>
    Pair<Sequence<T>*, Sequence<T2>*> Unzip(Sequence<Pair<T, T2>>* pairsSeq, Sequence<T>* sequence1, Sequence<T2>* sequence2);

    template<class T>
    Sequence<T>* Range(T start, T end, T step, Sequence<T>* sequence);

    template<class T, class T2>
    class ZipEnumerator : public IEnumerator<Pair<T, T2>>
    {
    private:
        IEnumerator<T>* enumA;
        IEnumerator<T2>* enumB;
        Pair<T, T2> current;

    public:
        ZipEnumerator(IEnumerator<T>* a, IEnumerator<T2>* b);
        ~ZipEnumerator() override;

        Pair<T, T2> GetCurrent() override;

        bool MoveNext() override;

        void Reset() override;
    };

    template<class T, class T2>
    ZipEnumerator<T, T2>* Zip(Sequence<T>* a, Sequence<T2>* b);

    template<class T>
    class SplitEnumerator : public IEnumerator<Sequence<T>*>
    {
    private:
        IEnumerator<T>* source;
        std::function<bool(T)> predicate;
        Sequence<T>* prototype;
        Sequence<T>* current;
        bool done;

    public:
        SplitEnumerator(IEnumerator<T>* src, std::function<bool(T)> predicate, Sequence<T>* proto);
        
        ~SplitEnumerator() override;

        Sequence<T>* GetCurrent() override;
        bool MoveNext() override;
        void Reset() override;
    };

    template<class T>
    SplitEnumerator<T>* Split(Sequence<T>* seq, std::function<bool(T)> predicate, Sequence<T>* prototype);

    template<class T>
    Sequence<T>* FlatMap(Sequence<T>* seq, std::function<Sequence<T>*(T)> function);

    template<class T>
    Sequence<T>* Skip(Sequence<T>* seq, size_t count);

    template<class T>
    Sequence<T>* Splice(Sequence<T>* seq, size_t index, size_t count, Sequence<T>* insertSequence = nullptr);

    template<class T>
    Sequence<T>* Map(Sequence<T>* seq, std::function<T(T)> func);
    
    template<class T, class T2>
    Sequence<T2>* Map(Sequence<T>* seq, std::function<T2(T)> func, Sequence<T2>* proto);

    template<class T>
    Sequence<T>* Where(Sequence<T>* seq, std::function<bool(T)> predicate);

    template<class T, class T2>
    T2 Reduce(Sequence<T>* seq, std::function<T2(T2, T)> func);

    template<class T>
    Option<T> GetFirst(Sequence<T>* seq, std::function<bool(T)> func);

    template<class T>
    Option<T> GetLast(Sequence<T>* seq, std::function<bool(T)> func);
}

#include <string>

namespace SequenceUtils {

    template<class T, class T2>
    Pair<Sequence<T>*, Sequence<T2>*> Unzip(Sequence<Pair<T, T2>>* pairsSeq, Sequence<T>* sequence1, Sequence<T2>* sequence2)
    {
        if (pairsSeq == nullptr || sequence1 == nullptr || sequence2 == nullptr) {
            throw InvalidArgument("SequenceUtils::Unzip - Invalid Argument: One or more sequence pointers are null.");
        }

        IEnumerator<Pair<T, T2>>* iterator = pairsSeq->GetEnumerator();
        
        try {
            while (iterator->MoveNext()) {
                Pair<T, T> current = iterator->GetCurrent();
                
                sequence1 = sequence1->Append(current.first);
                sequence2 = sequence2->Append(current.second);
            }
        } catch (...) {
            delete iterator;
            throw;
        }
        
        delete iterator;
        return Pair<Sequence<T>*, Sequence<T2>*>(sequence1, sequence2);
    }

    template<class T>
    Sequence<T>* Range(T start, T end, T step, Sequence<T>* sequence)
    {
        if (sequence == nullptr) {
            throw InvalidArgument("SequenceUtils::Range - Invalid Argument: Output sequence pointer is null.");
        }
        if (step == 0) {
            throw InvalidArgument("SequenceUtils::Range - Invalid Argument: Step cannot be zero.");
        }

        if (step > 0) {
            for (T i = start; i <= end; i += step) {
                sequence = sequence->Append(i);
            }
        } else {
            for (T i = start; i >= end; i += step) {
                sequence = sequence->Append(i);
            }
        }

        return sequence;
    }

    template<class T, class T2>
    ZipEnumerator<T, T2>::ZipEnumerator(IEnumerator<T>* a, IEnumerator<T2>* b) : enumA(a), enumB(b) 
    {
        if (a == nullptr || b == nullptr) {
            throw InvalidArgument("SequenceUtils::ZipEnumerator - Invalid Argument: Enumerators cannot be null.");
        }
    }

    template<class T, class T2>
    ZipEnumerator<T, T2>::~ZipEnumerator()
    {
        delete enumA;
        delete enumB;
    }

    template<class T, class T2>
    Pair<T, T2> ZipEnumerator<T, T2>::GetCurrent()
    {
        return current;
    }

    template<class T, class T2>
    bool ZipEnumerator<T, T2>::MoveNext()
    {
        if (enumA->MoveNext() && enumB->MoveNext()) {
            current = Pair<T, T2>(enumA->GetCurrent(), enumB->GetCurrent());
            return true;
        }
        return false;
    }

    template<class T, class T2>
    void ZipEnumerator<T, T2>::Reset()
    {
        enumA->Reset();
        enumB->Reset();
    }

    template<class T, class T2>
    ZipEnumerator<T, T2>* Zip(Sequence<T>* a, Sequence<T>* b)
    {
        if (a == nullptr || b == nullptr) {
            throw InvalidArgument("SequenceUtils::Zip - Invalid Argument: Source sequences cannot be null.");
        }
        return new ZipEnumerator<T, T2>(a->GetEnumerator(), b->GetEnumerator());
    }

    template<class T>
    SplitEnumerator<T>::SplitEnumerator(IEnumerator<T>* src, std::function<bool(T)> predicate, Sequence<T>* proto)
        : source(src), predicate(predicate), prototype(proto), current(nullptr), done(false) 
    {
        if (src == nullptr || !predicate || proto == nullptr) {
            throw InvalidArgument("SequenceUtils::SplitEnumerator - Invalid Argument: Null dependencies provided.");
        }
    }

    template<class T>
    SplitEnumerator<T>::~SplitEnumerator()
    {
        delete source;
    }

    template<class T>
    Sequence<T>* SplitEnumerator<T>::GetCurrent()
    {
        return current;
    }

    template<class T>
    bool SplitEnumerator<T>::MoveNext()
    {
        if (done) return false;

        Sequence<T>* part = prototype->CreateEmpty();
        bool hitSeparator = false;

        try {
            while (source->MoveNext()) {
                T val = source->GetCurrent();
                
                if (predicate(val)) {
                    hitSeparator = true;
                    break;
                }
                
                part = part->Append(val);
            }
        } catch (...) {
            delete part;
            throw;
        }

        current = part;
        
        if (!hitSeparator) {
            done = true;
        }

        return true;
    }

    template<class T>
    void SplitEnumerator<T>::Reset()
    {
        source->Reset();
        current = nullptr;
        done = false;
    }

    template<class T>
    SplitEnumerator<T>* Split(Sequence<T>* seq, std::function<bool(T)> predicate, Sequence<T>* prototype)
    {
        if (seq == nullptr || !predicate || prototype == nullptr) {
            throw InvalidArgument("SequenceUtils::Split - Invalid Argument: Null dependencies provided.");
        }
        return new SplitEnumerator<T>(seq->GetEnumerator(), predicate, prototype);
    }

    template<class T>
    Sequence<T>* FlatMap(Sequence<T>* seq, std::function<Sequence<T>*(T)> function) 
    {
        if (seq == nullptr || !function) {
            throw InvalidArgument("SequenceUtils::FlatMap - Invalid Argument: Null pointer or uninitialized function passed.");
        }

        Sequence<T>* result = seq->CreateEmpty();

        try {
            size_t length = seq->GetLength();
            
            for (size_t i = 0; i < length; ++i) {
                Sequence<T>* mappedSeq = function(seq->Get(i));
                
                if (mappedSeq != nullptr) {
                    size_t mappedLength = mappedSeq->GetLength();
                    
                    for (size_t j = 0; j < mappedLength; ++j) {
                        result = result->Append(mappedSeq->Get(j));
                    }
                    
                    delete mappedSeq;
                }
            }
        } catch (...) {
            delete result;
            throw;
        }

        return result;
    }

    template<class T>
    Sequence<T>* Skip(Sequence<T>* seq, size_t count)
    {
        if (seq == nullptr) {
            throw InvalidArgument("SequenceUtils::Skip - Invalid Argument: Source sequence pointer is null.");
        }

        Sequence<T>* result = seq->CreateEmpty();
        size_t length = seq->GetLength();

        if (count >= length) {
            return result; 
        }

        try {
            for (size_t i = count; i < length; ++i) {
                result = result->Append(seq->Get(i));
            }
        } catch (...) {
            delete result;
            throw;
        }

        return result;
    }

    template<class T>
    Sequence<T>* Splice(Sequence<T>* seq, size_t index, size_t count, Sequence<T>* insertSequence)
    {
        if (seq == nullptr) {
            throw InvalidArgument("SequenceUtils::Splice - Invalid Argument: Source sequence pointer is null.");
        }

        size_t length = seq->GetLength();

        if (index > length) {
            throw OutOfRange("SequenceUtils::Splice - Out of Range: Index " + std::to_string(index) + 
                             " is greater than the sequence length " + std::to_string(length) + ".");
        }

        Sequence<T>* result = seq->CreateEmpty();

        try {
            for (size_t i = 0; i < index; ++i) {
                result = result->Append(seq->Get(i));
            }

            if (insertSequence != nullptr) {
                size_t insertLength = insertSequence->GetLength();
                for (size_t i = 0; i < insertLength; ++i) {
                    result = result->Append(insertSequence->Get(i));
                }
            }

            size_t resumeIndex = index + count;
            
            if (resumeIndex < length) {
                for (size_t i = resumeIndex; i < length; ++i) {
                    result = result->Append(seq->Get(i));
                }
            }
        } catch (...) {
            delete result;
            throw;
        }

        return result;
    }

    template<class T>
    Sequence<T>* Map(Sequence<T>* seq, std::function<T(T)> func)
    {
        if (seq == nullptr) throw InvalidArgument("SequenceUtils::Map - Sequence is null.");
        if (!func) throw InvalidArgument("SequenceUtils::Map - std::function is empty.");

        Sequence<T>* result = seq->CreateEmpty();
        try {
            size_t length = seq->GetLength();
            for (size_t i = 0; i < length; ++i) {
                Sequence<T>* newResult = result->Append(func(seq->Get(i)));
                
                if (newResult != result) {
                    delete result;
                }
                result = newResult;
            }
        }
        catch (...) {
            delete result;
            throw;
        }

        return result;
    }

    template<class T, class T2>
    Sequence<T2>* Map(Sequence<T>* seq, std::function<T2(T)> func, Sequence<T2>* proto)
    {
        if (seq == nullptr || proto == nullptr) throw InvalidArgument("SequenceUtils::MapT2 - Sequence or Proto is null.");
        if (!func) throw InvalidArgument("SequenceUtils::MapT2 - std::function is empty.");

        Sequence<T2>* result = proto->CreateEmpty();
        try {
            size_t length = seq->GetLength();
            for (size_t i = 0; i < length; ++i) {
                Sequence<T2>* newResult = result->Append(func(seq->Get(i)));
                
                if (newResult != result) {
                    delete result;
                }
                result = newResult;
            }
        }
        catch (...) {
            delete result;
            throw;
        }

        return result;
    }

    template<class T>
    Sequence<T>* Where(Sequence<T>* seq, std::function<bool(T)> predicate)
    {
        if (seq == nullptr) throw InvalidArgument("SequenceUtils::Where - Sequence is null.");
        if (!predicate) throw InvalidArgument("SequenceUtils::Where - std::function is empty.");

        Sequence<T>* result = seq->CreateEmpty();
        try {
            size_t length = seq->GetLength();
            for (size_t i = 0; i < length; ++i) {
                T value = seq->Get(i);
                if (predicate(value)) {
                    Sequence<T>* newResult = result->Append(value);
                    
                    if (newResult != result) {
                        delete result;
                    }
                    result = newResult;
                }
            }
        }
        catch (...) {
            delete result;
            throw;
        }

        return result;
    }

    template<class T, class T2>
    T2 Reduce(Sequence<T>* seq, std::function<T2(T2, T)> func)
    {
        if (seq == nullptr) throw InvalidArgument("SequenceUtils::Reduce - Sequence is null.");
        if (!func) throw InvalidArgument("SequenceUtils::Reduce - std::function is empty.");
        
        size_t length = seq->GetLength();
        if (length == 0) {
            throw OutOfRange("SequenceUtils::Reduce - Cannot reduce an empty sequence.");
        }

        T2 result = static_cast<T2>(seq->Get(0));

        for (size_t i = 1; i < length; ++i) {
            result = func(result, seq->Get(i));
        }

        return result;
    }

    template<class T>
    Option<T> GetFirst(Sequence<T>* seq, std::function<bool(T)> func)
    {
        if (seq == nullptr) throw InvalidArgument("SequenceUtils::GetFirst - Sequence is null.");
        if (!func) throw InvalidArgument("SequenceUtils::GetFirst - Function is null.");
        
        size_t length = seq->GetLength();
        for (size_t index = 0; index < length; ++index) {
            T value = seq->Get(index);
            if (func(value)) {
                return Option<T>(value);
            }
        }
        return Option<T>();
    }

    template<class T>
    Option<T> GetLast(Sequence<T>* seq, std::function<bool(T)> func)
    {
        if (seq == nullptr) throw InvalidArgument("SequenceUtils::GetLast - Sequence is null.");
        if (!func) throw InvalidArgument("SequenceUtils::GetLast - Function is null.");
        
        size_t length = seq->GetLength();
        for (size_t index = length; index > 0; --index) {
            T value = seq->Get(index - 1);
            if (func(value)) {
                return Option<T>(value);
            }
        }
        return Option<T>();
    }
}
