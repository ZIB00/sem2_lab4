#pragma once

#include <cstddef>

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
    virtual bool   IsCanSeek()     const { return true; }
    virtual size_t Seek(size_t index)    = 0;
    virtual bool   IsCanGoBack()   const { return true; }

    virtual void Open() {}
    virtual void Close() {}
};

template< class T >
class IStreamWriteSrc
{
public:
    virtual ~IStreamWriteSrc() = default;

    virtual size_t GetPosition() const = 0;

    virtual size_t Write(T item) = 0;
    virtual void   Open()  {}
    virtual void   Close() {}
};