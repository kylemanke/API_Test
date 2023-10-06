#ifndef STUB_HTTP_HTTP_DEFINES_H
#define STUB_HTTP_HTTP_DEFINES_H

#include <string>

#include "robin_hood.h"

#include "types.h"
#include "http_maps.h"
#include "http_status_codes.h"

#define CRLF "\r\n"
#define SP " "

// Define needed enums
enum class HTTPMethod {
    kGet,
    kHead,
    kPost,
    kPut,
    kDelete,
    kConnect,
    kOptions,
    kTrace,
    kPatch
};

enum class HTTPVersion {
    kHTTP_0_9,
    kHTTP_1_0,
    kHTTP_1_1,
    kHTTP_2_0
};

enum class HTTPMessageType {kResponse, kRequest};

enum class HTTPStatusCode {
    k100, k101, k102, k103,
    k200, k201, k202, k203, k204, k205, k206, k207, k208, k226,
    k300, k301, k302, k303, k304, k307, k308, 
    k400, k401, k402, k403, k404, k405, k406, k407, k408, k409, k410, k411, k412, k413, 
    k414, k415, k416, k417, k418, k421, k422, k423, k424, k426, k428, k429, k431, k451, 
    k500, k501, k502, k503, k504, k505, k506, k507, k508, k510, k511
};

/// @brief Struct representing the start line of an HTTP request
struct RequestStartLine {
    HTTPMethod method_;
    std::string request_target_;
    HTTPVersion version_;

    std::string fmt() {
        std::string str = HTTPMethodList[static_cast<int>(method_)] + SP;
        str += request_target_ + SP;
        str += HTTPVersionList[static_cast<int>(version_)] + CRLF;
        return str;
    }
};

/// @brief Struct representing HTTP request
struct HTTPRequest {
    RequestStartLine start_line_;
    robin_hood::unordered_flat_map<std::string, std::string> headers_;
    std::string body_; // TODO: kind of want it to be char*, seems much cleaner

    /// @brief Function to turn HTTPRequest Struct into its string version
    /// @return Formatted HTTP request message
    std::string fmt() {
        // Begin with the start line
        std::string str = start_line_.fmt();

        // Go through all the headers
        robin_hood::unordered_flat_map<std::string, std::string>::iterator it;
        for (it = headers_.begin(); it != headers_.end(); it++) {
            str += it->first + ": " + it->second + CRLF;
        }
        str += CRLF;

        // Add the body
        str += body_;

        return str;
    }
};

/// @brief Struct representing status line of HTTP Response
struct ResponseStatusLine {
    HTTPVersion version_;
    HTTPStatusCode status_code_;

    std::string fmt() {
        std::string str = HTTPVersionList[static_cast<int>(version_)] + SP;
        str += StatusCodeList[static_cast<int>(status_code_)].fmt() + CRLF;
        return str;
    }
};

/// @brief Struct representing HTTP response
struct HTTPResponse {
    ResponseStatusLine status_line_;
    robin_hood::unordered_flat_map<std::string, std::string> headers_;
    std::string body_;

    std::string fmt() {
        // Begin with the start line
        std::string str = status_line_.fmt();

        // Append all headers
        robin_hood::unordered_flat_map<std::string, std::string>::iterator it;
        for (it = headers_.begin(); it != headers_.end(); it++) {
            str += it->first + ": " + it->second + CRLF;
        }
        str += CRLF;

        // Append the body
        str += body_;

        return str;
    }
};

#endif