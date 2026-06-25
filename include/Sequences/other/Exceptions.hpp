#pragma once

#include <exception>
#include <string>

class BaseError : public std::exception
{
protected:
    std::string message;
public:
    BaseError(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

class InvalidArgument : public BaseError
{
public:
    InvalidArgument(const std::string& msg) : BaseError("InvalidArgument: " + msg) {}
};

class OutOfRange : public BaseError
{
public:
    OutOfRange(const std::string& msg) : BaseError("OutOfRange: " + msg) {}
};

class LogicError : public BaseError
{
public:
    LogicError(const std::string& msg) : BaseError("LogicError: " + msg) {}
};

class OtherError : public BaseError
{
public:
    OtherError(const std::string& msg) : BaseError("OtherError: " + msg) {}
};

class BadOptionalAccess : public BaseError 
{
public:
    BadOptionalAccess() : BaseError("bad optional access: optional is empty") {}
};

class NotSupportedException : public BaseError 
{
public:
    NotSupportedException() : BaseError("Not supported") {}
};

class EndOfStream : public BaseError 
{
public:
    EndOfStream(const std::string& msg) : BaseError("EndOfStream: " + msg) {}
};

class SourceIsNotOpen : public BaseError
{
public:
    SourceIsNotOpen(const std::string& msg) : BaseError("SourceIsNotOpen: " + msg) {}
};