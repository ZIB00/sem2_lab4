#pragma once

#include <memory>
#include <string>
#include <fstream>
#include <memory>
#include <functional>
#include <string>

#include "../Sequences/sequences/Sequence.hpp"
#include "IStreams.hpp"

template< class T >
class SeqWriteSrc : public IStreamWriteSrc<T>
{
private:
    std::shared_ptr<Sequence<T>> seq;
    size_t currentIndex;

public:
    SeqWriteSrc( std::shared_ptr<Sequence<T>> seq, size_t currentIndex ) 
        : seq(seq), currentIndex(currentIndex) {}

    size_t GetPosition() const override { return currentIndex; }

    size_t Write(T item) override { 
        seq->Append(item);
        return ++currentIndex; 
    }
};

template< class T >
class FileWriteSrc : public IStreamWriteSrc<T>
{
private:
    std::ofstream out;
    std::function<std::string(const T& item)> ser;
    std::string filePath;
    size_t currentIndex;

public:
    FileWriteSrc( const std::string& filePath, std::function<std::string(const T& item)> ser ) 
        : ser(ser), filePath(filePath), currentIndex(0) {
            out.open(filePath);
        }
    ~FileWriteSrc() {
        out.close();
    }
    
    size_t GetPosition() const override { return currentIndex; }

    size_t Write(T item) override {
        out << ser(item) << " ";
        return ++currentIndex;
    }
};