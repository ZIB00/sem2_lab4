#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <fstream>
#include <memory>
#include <functional>
#include <string>

#include "../LazySequence/LazySequence.hpp"
#include "../Sequences/sequences/Sequence.hpp"


class IStream
{
public:
    virtual ~IStream() = default;

    virtual size_t GetPosition() const = 0;
    
    virtual void Open()  = 0;
    virtual void Close() = 0;
};

template< class T >
class IStreamReadSrc
{
public:
    virtual ~IStreamReadSrc() = default;

    virtual bool   IsEndOfStream() const = 0; 
    virtual T      Read()                = 0;
    virtual size_t GetPosition()   const = 0;
    virtual bool   IsCanSeek()     const = 0; 
    virtual size_t Seek(size_t index)    = 0;
    virtual bool   IsCanGoBack()   const = 0;

    virtual void Open() {}
    virtual void Close() {}
};

template< class T >
class SeqReadSrc : public IStreamReadSrc<T>
{
private:
    std::shared_ptr<Sequence<T>> seq;
    size_t currentIndex;

public:
    SeqReadSrc(std::shared_ptr<Sequence<T>> seq, size_t currentIndex ) 
        : seq(seq), currentIndex(currentIndex) {}

    bool IsEndOfStream() const override { return (seq->GetLength() - 1) == currentIndex; }
    T Read() override { 
        if( seq->GetLength() <= currentIndex ) throw EndOfStream ("достигнут конец потока");
        return seq->Get(currentIndex++);
    }
    size_t GetPosition()   const override { return currentIndex + 1; }
    bool IsCanSeek() const override { return true; }
    size_t Seek(size_t index) override { 
        if( seq->GetLength() <= index ) {
            currentIndex = seq->GetLength() - 1; //Пробует перейти на заданную позицию, если возможно. Если позиция по какой-либо причине недостижима, может выбросить исключение; !!!! иногда –  переходит на  ближайшую  возможную позицию  и  возвращает соответствующий индекс. !!!
        } else { currentIndex = index; }
        return currentIndex;
    }  
    bool IsCanGoBack() const override { return true; }
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
        return ( seq->GetMaterializedCount() - 1 ) == currentIndex;
    }
    T Read() override {
        if( !seq->GetLength().IsInfinite() && 
            seq->GetMaterializedCount() - 1 == currentIndex) throw EndOfStream ("достигнут конец потока");
        return seq->Get(currentIndex++);
    }
    size_t GetPosition()   const override { return currentIndex; }
    bool   IsCanSeek()     const override { return true; }
    size_t Seek(size_t index)    override { 
        if( !seq->GetLength().IsInfinite() && 
            seq->GetMaterializedCount() - 1 == currentIndex ) {
            currentIndex = seq->GetMaterializedCount() - 1; //Пробует перейти на заданную позицию, если возможно. Если позиция по какой-либо причине недостижима, может выбросить исключение; !!!! иногда –  переходит на  ближайшую  возможную позицию  и  возвращает соответствующий индекс. !!!
        } else { currentIndex = index; }
        return currentIndex;
    }  
    bool   IsCanGoBack()   const override { return true; }
};

template< class T >
class FileReadSrc : public IStreamReadSrc<T>
{
private:
    std::ifstream in;
    std::function<T(const std::string& str)> deser;
    std::string filePath;

    T FromStr( const std::string& str );

public:
    FileReadSrc( const std::string& filePath, std::function<T(std::string str)> deser ) 
        : filePath(filePath), deser(deser) {};

    bool   IsEndOfStream() const override;
    T      Read()                override;
    size_t GetPosition()   const override;
    bool   IsCanSeek()     const override;
    size_t Seek(size_t index)    override;
    bool   IsCanGoBack()   const override;

    void Open()  override;
    void Close() override;
};

template< class T >
class StringReadSrc : public IStreamReadSrc<T>
{
private:
    std::shared_ptr<std::string> str;
    std::function<T(const std::string& str)> deser;
    size_t currentIndex;

public:
    StringReadSrc( std::shared_ptr<std::string> str, std::function<T(std::string str)> deser, size_t currentIndex ) 
        : str(str), deser(deser), currentIndex(currentIndex) {}

    bool   IsEndOfStream () const override; 
    T      Read()                 override;
    size_t GetPosition()    const override;
    bool   IsCanSeek()      const override; 
    size_t Seek(size_t index)     override;
    bool   IsCanGoBack()    const override;

    void Open()  override;
    void Close() override;
};

template< class T >
class ReadOnlyStream : public IStream
{
private:
    std::shared_ptr<IStreamReadSrc<T>> src;
    bool isOpen;

public:
    ReadOnlyStream( std::shared_ptr<std::string> str, std::function< T(const std::string& str) > deser )
        : src( std::make_shared<StringReadSrc<T>>(str, deser, 0) ), isOpen(false) {}
    ReadOnlyStream( const std::string& filePath,      std::function< T(const std::string& str) > deser )
        : src( std::make_shared<FileReadSrc<T>>(filePath, deser) ), isOpen(false) {}
    ReadOnlyStream( std::shared_ptr<Sequence<T>> seq )     
        : src( std::make_shared<SeqReadSrc<T>>(seq, 0) ),     isOpen(false) {}
    ReadOnlyStream( std::shared_ptr<LazySequence<T>> seq ) 
        : src( std::make_shared<LazySeqReadSrc<T>>(seq, 0) ), isOpen(false) {}

    bool   IsEndOfStream () const;
    T      Read();
    size_t GetPosition()    const override;
    bool   IsCanSeek()      const; 
    size_t Seek(size_t index);
    bool   IsCanGoBack()    const;

    void Open()  override;
    void Close() override;
};

template< class T >
class IStreamWriteSrc
{
public:
    virtual ~IStreamWriteSrc() = default;

    virtual size_t GetPosition() const = 0;

    virtual size_t Write(T item) = 0;
    virtual void   Open()        = 0;
    virtual void   Close()       = 0;
};

template< class T >
class SeqWriteSrc : public IStreamWriteSrc<T>
{
private:
    std::shared_ptr<Sequence<T>> seq;
    size_t currentIndex;

public:
    SeqWriteSrc( std::shared_ptr<Sequence<T>> seq, size_t currentIndex ) 
        : seq(seq), currentIndex(currentIndex) {}

    size_t GetPosition() const override;

    size_t Write(T item) override;
    void   Open()        override;
    void   Close()       override;
};

template< class T >
class FileWriteSrc : public IStreamWriteSrc<T>
{
private:
    std::ofstream out;
    std::function<std::string(const T& item)> ser;
    std::string filePath;

    std::string ToStr( T item );

public:
    FileWriteSrc( const std::string& filePath, std::function<std::string(const T& item)> ser ) 
        : ser(ser), filePath(filePath) {}
    
    size_t GetPosition() const override;

    size_t Write(T item) override;
    void   Open()        override;
    void   Close()       override;
};

template< class T >
class WriteOnlyStream : public IStream
{
private:
    std::shared_ptr<IStreamWriteSrc<T>> src;
    bool isOpen;

public:
    WriteOnlyStream<T>( const std::string& filePath, std::function< std::string(const T& item) > ser )
        : src( std::make_shared<FileWriteSrc<T>>(filePath, ser) ), isOpen(false) {}
    WriteOnlyStream( std::shared_ptr<Sequence<T>> seq ) 
        : src( std::make_shared<SeqWriteSrc<T>>(seq, 0) ), isOpen(false) {}

    size_t GetPosition() const override;

    size_t Write(T item) override;
    void   Open()        override;
    void   Close()       override;
};