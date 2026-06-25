#pragma once
#include <memory>
#include <QString>

#include "../include/LazySequence/LazySequence.hpp"
#include "../include/Sequences/sequences/ArraySequence.hpp"

class ISequenceWrapper
{
public:
    virtual ~ISequenceWrapper() = default;

    virtual QString GetFirst() = 0;
    virtual QString GetLast() = 0;
    virtual QString Get(size_t index) = 0;
    virtual bool IsLengthInfinite() = 0;
    virtual size_t GetLengthSize() = 0;
    virtual size_t GetMaterializedCount() = 0;
    virtual QString GetNext() = 0;
    virtual bool TryGetNext(QString& outValue) = 0;
    virtual std::shared_ptr<ISequenceWrapper> Set(const QString& value, size_t index) = 0;
    virtual std::shared_ptr<ISequenceWrapper> Append(const QString& value) = 0;
    virtual std::shared_ptr<ISequenceWrapper> Prepend(const QString& value) = 0;
    virtual std::shared_ptr<ISequenceWrapper> InsertAt(const QString& value, size_t index) = 0;
    virtual std::shared_ptr<ISequenceWrapper> Remove(size_t index) = 0;
    virtual std::shared_ptr<ISequenceWrapper> MapByName(const QString& funcName) = 0;
    virtual std::shared_ptr<ISequenceWrapper> WhereByName(const QString& funcName) = 0;
    virtual QString ReduceByName(const QString& funcName) = 0;
    virtual QString ZipWith(std::shared_ptr<ISequenceWrapper> other, size_t count) = 0;
};

inline int ParseValue(const QString& text, int)
{
    return text.toInt();
}

inline double ParseValue(const QString& text, double)
{
    return text.toDouble();
}

inline QString FormatValue(int value)
{
    return QString::number(value);
}

inline QString FormatValue(double value)
{
    return QString::number(value, 'g', 10);
}

template <typename T>
class SequenceWrapperImpl : public ISequenceWrapper
{
private:
    std::shared_ptr<LazySequence<T>> seq;

    static T MulByTwoFunc(T x) { return x * 2; }
    static T SquareFunc(T x) { return x * x; }
    static T IncrementFunc(T x) { return x + 1; }

    static bool IsPositiveFunc(T x) { return x > T(); }
    static bool IsEvenFunc(T x) { return static_cast<size_t>(x) % 2 == 0; }

    static T SumFunc(T a, T b) { return a + b; }
    static T ProductFunc(T a, T b) { return a * b; }
    static T MaxFunc(T a, T b) { return a > b ? a : b; }
    static T MinFunc(T a, T b) { return a < b ? a : b; }

    template <typename T2>
    QString ZipWithTyped(std::shared_ptr<LazySequence<T2>> otherSeq, size_t count)
    {
        auto zipped = Zip<T, T2>(seq, otherSeq);
        QString result;
        for (size_t i = 0; i < count; ++i) {
            try {
                auto p = zipped.Get(i);
                result += "(" + FormatValue(p.first) + ", " + FormatValue(p.second) + ") ";
            } catch (const BaseError&) {
                break;
            }
        }
        return result;
    }

public:
    explicit SequenceWrapperImpl(std::shared_ptr<LazySequence<T>> sequence) : seq(sequence) {}

    std::shared_ptr<LazySequence<T>> GetRawSequence()
    {
        return seq;
    }

    QString GetFirst() override
    {
        return FormatValue(seq->GetFirst());
    }

    QString GetLast() override
    {
        return FormatValue(seq->GetLast());
    }

    QString Get(size_t index) override
    {
        return FormatValue(seq->Get(index));
    }

    bool IsLengthInfinite() override
    {
        try { return seq->GetLength().IsInfinite(); } catch (...) { return false; }
    }

    size_t GetLengthSize() override
    {
        try { return seq->GetLength().GetSize(); } catch (...) { return 0; }
    }

    size_t GetMaterializedCount() override
    {
        return seq->GetMaterializedCount();
    }

    QString GetNext() override
    {
        return FormatValue(seq->GetNext());
    }

    bool TryGetNext(QString& outValue) override
    {
        auto opt = seq->TryGetNext();
        if (opt.HasValue()) {
            outValue = FormatValue(opt.Value());
            return true;
        }
        return false;
    }

    std::shared_ptr<ISequenceWrapper> Set(const QString& value, size_t index) override
    {
        T v = ParseValue(value, T());
        auto newSeq = std::make_shared<LazySequence<T>>(seq->Set(v, index));
        return std::make_shared<SequenceWrapperImpl<T>>(newSeq);
    }

    std::shared_ptr<ISequenceWrapper> Append(const QString& value) override
    {
        T v = ParseValue(value, T());
        auto newSeq = std::make_shared<LazySequence<T>>(seq->Append(v));
        return std::make_shared<SequenceWrapperImpl<T>>(newSeq);
    }

    std::shared_ptr<ISequenceWrapper> Prepend(const QString& value) override
    {
        T v = ParseValue(value, T());
        auto newSeq = std::make_shared<LazySequence<T>>(seq->Prepend(v));
        return std::make_shared<SequenceWrapperImpl<T>>(newSeq);
    }

    std::shared_ptr<ISequenceWrapper> InsertAt(const QString& value, size_t index) override
    {
        T v = ParseValue(value, T());
        auto newSeq = std::make_shared<LazySequence<T>>(seq->InsertAt(v, index));
        return std::make_shared<SequenceWrapperImpl<T>>(newSeq);
    }

    std::shared_ptr<ISequenceWrapper> Remove(size_t index) override
    {
        auto newSeq = std::make_shared<LazySequence<T>>(seq->Remove(index));
        return std::make_shared<SequenceWrapperImpl<T>>(newSeq);
    }

    std::shared_ptr<ISequenceWrapper> MapByName(const QString& funcName) override
    {
        LazySequence<T> result;
        if (funcName == "x * 2") {
            result = seq->Map(std::function<T(T)>(MulByTwoFunc));
        } else if (funcName == "x * x") {
            result = seq->Map(std::function<T(T)>(SquareFunc));
        } else if (funcName == "x + 1") {
            result = seq->Map(std::function<T(T)>(IncrementFunc));
        } else {
            result = *seq;
        }
        auto newSeq = std::make_shared<LazySequence<T>>(result);
        return std::make_shared<SequenceWrapperImpl<T>>(newSeq);
    }

    std::shared_ptr<ISequenceWrapper> WhereByName(const QString& funcName) override
    {
        LazySequence<T> result;
        if (funcName == "x > 0") {
            result = seq->Where(std::function<bool(T)>(IsPositiveFunc));
        } else if (funcName == "Чётные") {
            result = seq->Where(std::function<bool(T)>(IsEvenFunc));
        } else {
            result = *seq;
        }
        auto newSeq = std::make_shared<LazySequence<T>>(result);
        return std::make_shared<SequenceWrapperImpl<T>>(newSeq);
    }

    QString ReduceByName(const QString& funcName) override
    {
        if (funcName == "Сумма") {
            return FormatValue(seq->Reduce(std::function<T(T, T)>(SumFunc)));
        } else if (funcName == "Произведение") {
            return FormatValue(seq->Reduce(std::function<T(T, T)>(ProductFunc)));
        } else if (funcName == "Максимум") {
            return FormatValue(seq->Reduce(std::function<T(T, T)>(MaxFunc)));
        } else if (funcName == "Минимум") {
            return FormatValue(seq->Reduce(std::function<T(T, T)>(MinFunc)));
        }
        return "";
    }

    QString ZipWith(std::shared_ptr<ISequenceWrapper> other, size_t count) override
    {
        auto otherInt = std::dynamic_pointer_cast<SequenceWrapperImpl<int>>(other);
        if (otherInt) {
            return ZipWithTyped<int>(otherInt->GetRawSequence(), count);
        }

        auto otherDouble = std::dynamic_pointer_cast<SequenceWrapperImpl<double>>(other);
        if (otherDouble) {
            return ZipWithTyped<double>(otherDouble->GetRawSequence(), count);
        }

        return "";
    }

};