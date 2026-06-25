#pragma once

#include "ReadOnlySrc.hpp"
#include "WriteOnlySrc.hpp"

template< class T >
class ReadOnlyStream : public IStream
{
private:
    std::shared_ptr<IStreamReadSrc<T>> src;
    bool isOpen;

public:
    ReadOnlyStream( std::shared_ptr<std::string> str, std::function< T(const std::string& str) > deser )
        : src( std::make_shared<StringReadSrc<T>>(str, deser) ), isOpen(true) {}
    ReadOnlyStream( const std::string& filePath,      std::function< T(const std::string& str) > deser )
        : src( std::make_shared<FileReadSrc<T>>(filePath, deser) ), isOpen(false) {}
    ReadOnlyStream( std::shared_ptr<Sequence<T>> seq )     
        : src( std::make_shared<SeqReadSrc<T>>(seq, 0) ),     isOpen(true) {}
    ReadOnlyStream( std::shared_ptr<LazySequence<T>> seq ) 
        : src( std::make_shared<LazySeqReadSrc<T>>(seq, 0) ), isOpen(true) {}

    bool IsEndOfStream() const { 
        if(!isOpen) throw SourceIsNotOpen("Source is not open");
        return src->IsEndOfStream();
    }
    T Read() { 
        if( !isOpen ) throw SourceIsNotOpen("Source is not open");
        return src->Read(); 
    }
    size_t GetPosition() const override { 
        if( !isOpen ) throw SourceIsNotOpen("Source is not open");
        return src->GetPosition();
    }
    bool IsCanSeek() const { 
        if( !isOpen ) throw SourceIsNotOpen("Source is not open");
        return src->IsCanSeek();
    }
    size_t Seek(size_t index) { 
        if( !isOpen ) throw SourceIsNotOpen("Source is not open");
        return src->Seek(index);
    }
    bool IsCanGoBack() const { 
        if( !isOpen ) throw SourceIsNotOpen("Source is not open");
        return src->IsCanGoBack();
    }

    void Open()  override { 
        if( isOpen ) throw LogicError("Source is already open");

        src->Open();
        isOpen = true;
    }
    void Close() override { 
        if( !isOpen ) throw SourceIsNotOpen("Source is not open");
        src->Close();
        isOpen = false;
    }
};

template< class T >
class WriteOnlyStream : public IStream
{
private:
    std::shared_ptr<IStreamWriteSrc<T>> src;
    bool isOpen;

public:
    WriteOnlyStream( const std::string& filePath, std::function< std::string(const T& item) > ser )
        : src( std::make_shared<FileWriteSrc<T>>(filePath, ser) ), isOpen(false) {}
    WriteOnlyStream( std::shared_ptr<Sequence<T>> seq ) 
        : src( std::make_shared<SeqWriteSrc<T>>(seq, 0) ), isOpen(true) {}

    size_t GetPosition() const override { 
        if( !isOpen ) throw SourceIsNotOpen("Source is not open");
        return src->GetPosition();
    }

    size_t Write(T item) { 
        if( !isOpen ) throw SourceIsNotOpen("Source is not open");
        return src->Write(item);
    }
    void Open()  override { 
        if( isOpen ) throw LogicError("Source is already open");

        src->Open();
        isOpen = true;
    }
    void Close() override { 
        if( !isOpen ) throw SourceIsNotOpen("Source is not open");
        
        src->Close();
        isOpen = false;
    }
};