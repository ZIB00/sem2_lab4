#pragma once

#include <memory>
#include <string>
#include <fstream>
#include <memory>
#include <functional>
#include <string>
#include <sstream>

#include "../LazySequence/LazySequence.hpp"
#include "../Sequences/sequences/Sequence.hpp"
#include "IStreams.hpp"

template< class T >
class SeqReadSrc : public IStreamReadSrc<T>
{
private:
    std::shared_ptr<Sequence<T>> seq;
    size_t currentIndex;

public:
    SeqReadSrc(std::shared_ptr<Sequence<T>> seq, size_t currentIndex ) 
        : seq(seq), currentIndex(currentIndex) {}

    bool IsEndOfStream() const override { return (seq->GetLength()) == currentIndex; }
    T Read() override { 
        if( seq->GetLength() <= currentIndex ) throw EndOfStream ("достигнут конец потока");
        return seq->Get(currentIndex++);
    }
    size_t GetPosition()   const override { return currentIndex; }
    size_t Seek(size_t index) override { 
        if( seq->GetLength() <= index ) {
            currentIndex = seq->GetLength() - 1; //Пробует перейти на заданную позицию, если возможно. Если позиция по какой-либо причине недостижима, может выбросить исключение; !!!! иногда –  переходит на  ближайшую  возможную позицию  и  возвращает соответствующий индекс. !!!
        } else { currentIndex = index; }
        return currentIndex;
    }  
};

template< class T >
class LazySeqReadSrc : public IStreamReadSrc<T>
{
private:
    std::shared_ptr<LazySequence<T>> seq;
    size_t currentIndex;

public: 
    LazySeqReadSrc( std::shared_ptr<LazySequence<T>> seq, size_t currentIndex ) 
        : seq(seq), currentIndex(currentIndex) {}

    bool IsEndOfStream() const override { 
        if( seq->GetLength().IsInfinite() ) return false;
        return ( seq->GetLength().GetSize() ) == currentIndex;
    }
    T Read() override {
        if( !seq->GetLength().IsInfinite() && 
            seq->GetLength().GetSize() == currentIndex) throw EndOfStream ("достигнут конец потока");
        return seq->Get(currentIndex++);
    }
    size_t GetPosition()   const override { return currentIndex; }
    size_t Seek(size_t index)    override { 
        if( !seq->GetLength().IsInfinite() && 
            seq->GetLength().GetSize() <= index ) {
            currentIndex = seq->GetLength().GetSize() - 1; //Пробует перейти на заданную позицию, если возможно. Если позиция по какой-либо причине недостижима, может выбросить исключение; !!!! иногда –  переходит на  ближайшую  возможную позицию  и  возвращает соответствующий индекс. !!!
        } else { currentIndex = index; }
        return currentIndex;
    } 
};

template< class T >
class FileReadSrc : public IStreamReadSrc<T>
{
private:
    std::ifstream in;
    std::function<T(const std::string& str)> deser;
    std::string filePath;
    size_t currentIndex;
    std::string part;

public:
    FileReadSrc( const std::string& filePath, std::function<T(const std::string& str)> deser ) 
        : filePath(filePath), deser(deser), currentIndex(0) {};

    bool IsEndOfStream() const override { return in.fail(); }
    T Read() override { 
        if( IsEndOfStream() && in.fail() ) throw EndOfStream("End of stream");

        T res = deser(part);
        ++currentIndex;
        in >> part;
        return res;
    }
    size_t GetPosition() const override { return currentIndex; }
    size_t Seek(size_t index)  override {
        if( currentIndex > index ) {
            currentIndex = 0;
            in.clear();
            in.seekg(0);
            in >> part;
        }

        while (IsEndOfStream() == false && currentIndex < index) { 
            in >> part;
            ++currentIndex; 
        }
        return currentIndex;
    }

    void Open() override { 
        in.open(filePath);
        if( !in.is_open() ) throw OtherError("File did not open");
        in >> part;
    }
    void Close() override { 
        in.clear();
        in.close(); 
        if( in.fail() ) throw OtherError("File did not close");
    }
};

template< class T >
class StringReadSrc : public IStreamReadSrc<T>
{
private:
std::shared_ptr<std::string> str;
    std::stringstream ss;
    std::function<T(const std::string& str)> deser;
    size_t currentIndex;
    std::string part;

public:
    StringReadSrc( std::shared_ptr<std::string> str, std::function<T(const std::string& ss)> deser) 
        : str(str), deser(deser), currentIndex(0) { 
            ss << *str; 
            ss >> part;
        }

    bool IsEndOfStream() const override { return ss.fail(); }
    T Read() override { 
        if( IsEndOfStream() && ss.fail() ) throw EndOfStream("End of stream");

        T res = deser(part);
        ++currentIndex;
        ss >> part;
        return res;
    }
    size_t GetPosition() const override { return currentIndex; }
    size_t Seek(size_t index)  override {
        if( currentIndex > index ) {
            currentIndex = 0;
            ss.clear();
            ss.seekg(0);
            ss >> part;
        }

        while (IsEndOfStream() == false && currentIndex < index) { 
            ss >> part;
            ++currentIndex; 
        }
        return currentIndex;
    }
};