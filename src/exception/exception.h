#ifndef STUB_EXCEPTION_EXCEPTION_H
#define STUB_EXCEPTION_EXCEPTION_H

#include <exception>
#include <cstdio>

#include "types.h"

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

#endif