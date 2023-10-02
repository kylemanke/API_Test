#ifndef STUB_EXCEPTION_EXCEPTION_H
#define STUB_EXCEPTION_EXCEPTION_H

#include <exception>
#include <cstdio>

#include "types.h"

/// @brief Exception throw when a socket error occurs
class SocketException : public std::exception 
{
public:
    SocketException(int32 error) : error_(error) {}
    
    virtual const char* what() {
        sprintf(str_, "Socket error: %d", error_);
        return str_;
    }

private:
    char str_[64];
    int32 error_;
};

class HTTPReadError : public std::exception
{
public:
    virtual const char* what() {
        return "Failed to read byte(s) from HTTPSocket";
    }
};

#endif