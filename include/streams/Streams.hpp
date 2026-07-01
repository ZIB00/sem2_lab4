#pragma once

#include "ReadOnlySrc.hpp"
#include "WriteOnlySrc.hpp"

template< class T >
class ReadOnlyStream : public IStream
{
private:
    std::shared_ptr<IStreamReadSrc<T>> src;

public:
    ReadOnlyStream( std::shared_ptr<std::string> str, std::function< T(const std::string& str) > deser )
        : src( std::make_shared<StringReadSrc<T>>(str, deser) ) {}
    ReadOnlyStream( const std::string& filePath,      std::function< T(const std::string& str) > deser )
        : src( std::make_shared<FileReadSrc<T>>(filePath, deser) ) {}
    ReadOnlyStream( std::shared_ptr<Sequence<T>> seq )     
        : src( std::make_shared<SeqReadSrc<T>>(seq, 0) ) {}
    ReadOnlyStream( std::shared_ptr<LazySequence<T>> seq ) 
        : src( std::make_shared<LazySeqReadSrc<T>>(seq, 0) ){}

    bool IsEndOfStream() const { return src->IsEndOfStream(); }
    T Read() {  return src->Read();  }
    size_t GetPosition() const override { return src->GetPosition(); }
    bool IsCanSeek() const {  return src->IsCanSeek(); }
    size_t Seek(size_t index) {  return src->Seek(index); }
    bool IsCanGoBack() const { return src->IsCanGoBack(); }
};

template< class T >
class WriteOnlyStream : public IStream
{
private:
    std::shared_ptr<IStreamWriteSrc<T>> src;
    bool isOpen;

public:
    WriteOnlyStream( const std::string& filePath, std::function< std::string(const T& item) > ser )
        : src( std::make_shared<FileWriteSrc<T>>(filePath, ser) ) {}
    WriteOnlyStream( std::shared_ptr<Sequence<T>> seq ) 
        : src( std::make_shared<SeqWriteSrc<T>>(seq, 0) ) {}

    size_t GetPosition() const override { return src->GetPosition(); }

    size_t Write(T item) { return src->Write(item); }
};