#ifndef STUB_HTTP_HTTP_STATUS_CODES_H
#define STUB_HTTP_HTTP_STATUS_CODES_H

#include <string>

struct StatusCode {
    std::string code_;
    std::string message_;

    std::string fmt() const {
        return code_ + " " + message_;
    }
};

extern const struct StatusCode StatusCodeList[];

#endif