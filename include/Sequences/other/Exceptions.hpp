#pragma once

#include <stdexcept>
#include <string>

class InvalidArgument : public std::invalid_argument
{
    public:
        using std::invalid_argument::invalid_argument;
};

class OutOfRange : public std::out_of_range
{
    public:
        using std::out_of_range::out_of_range;
};

class LogicError : public std::logic_error
{
    public:
        using std::logic_error::logic_error;
};

class OtherError : public std::exception
{
    public:
        using std::exception::exception;
};

class BadOptionalAccess : public std::exception 
{
    public:
    const char* what() const noexcept override {
        return "bad optional access: optional is empty";
    }
};

class NotSupportedException : public std::logic_error 
{
    public:
    using std::logic_error::logic_error;
    
    const char* what() const noexcept override {
        return "Not supported";
    }
};