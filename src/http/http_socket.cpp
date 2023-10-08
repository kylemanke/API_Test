#include <cstring>
#include <string>

#include "exception/exception.h"
#include "types.h"
#include "stub_defines.h"

#include "http_socket.h"

#define BUFFER_SIZE 2048

std::string to_lower(std::string str) {
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] += 32;
        }
    }
    return str;
}

int32 atoi(const std::string str) {
    int32 ret_val = 0;
    for (int i = str.size() - 1; i >= 0; i--) {
        ret_val *= 10;
        ret_val += str[i] - '0';
    }
    return ret_val;
}

HTTPSocket::HTTPSocket() 
    : socket_(-1)
{
    buffer_ = new char[BUFFER_SIZE];
    idx_ = 0;
    buffer_size_ = 0;
    buffer_max_ = BUFFER_SIZE;
}

HTTPSocket::HTTPSocket(Socket sock)
    : socket_(sock)
{
    buffer_ = new char[BUFFER_SIZE];
    idx_ = 0;
    buffer_size_ = 0;
    buffer_max_ = BUFFER_SIZE;
}

HTTPSocket::HTTPSocket(const char *host, const char *port)
{
    // Create the socket
    socket_.Connect(host, port);

    // Set up buffer
    buffer_ = new char[BUFFER_SIZE];
    idx_ = 0;
    buffer_size_ = 0;
    buffer_max_ = BUFFER_SIZE;
}

HTTPSocket::~HTTPSocket() {
    if (buffer_ != nullptr)
        delete[] buffer_;
}

void HTTPSocket::SetSocket(Socket sock) {
    if (socket_.Valid()) socket_.Shutdown();
    socket_ = sock;
}

void HTTPSocket::SetSocket(const char *host, const char *port) {
    if (socket_.Valid()) socket_.Shutdown();
    socket_ = Socket();
    socket_.Connect(host, port);
}

bool HTTPSocket::Valid() {
    return socket_.Valid();
}

void HTTPSocket::Shutdown() {
    socket_.Shutdown();
}

char HTTPSocket::NextByte() {
    char ret_char;

    if (idx_ >= buffer_size_) {
        // Must fetch more bytes to read
        idx_ = 0;
        buffer_size_ = socket_.Recv(buffer_, buffer_max_);
        if (buffer_size_ == 0)
            throw HTTPReadError();
    }

    ret_char = buffer_[idx_++];

    return ret_char;
}

char HTTPSocket::PeekByte() {
    char ret_char;

    if (idx_ >= buffer_size_) {
        idx_ = 0;
        buffer_size_ = socket_.Recv(buffer_, buffer_max_);
        if (buffer_size_ == 0)
            throw HTTPReadError();
    }

    return buffer_[idx_];
}

uint32 HTTPSocket::NextBytes(char *dest, uint32 size) {
    // Copy current buffer chars to dest
    uint32 bytes_read = 0;
    if (size <= (buffer_size_ - idx_)) {
        // There is enough info in the buffer
        memcpy(dest, buffer_ + idx_, size);
        idx_ += size;
        bytes_read = size;
    } else {
        // Take rest of buffer
        memcpy(dest, buffer_ + idx_, buffer_size_ - idx_);

        // Take what is needed until done
        while(bytes_read < size) {
            // Get new buffer
            buffer_size_ = socket_.Recv(buffer_, buffer_max_);
            idx_ = 0;
            if (buffer_size_ == 0)
                throw HTTPReadError();
            
            // Take what is needed
            uint32 read_amount = min(buffer_size_, size - bytes_read);
            memcpy(dest + bytes_read, buffer_, read_amount);
            idx_ += read_amount;
            bytes_read += read_amount;
        }
    }

    return bytes_read;
}

bool HTTPSocket::PendingBytes() {
    // Check for bytes in the buffer
    if (buffer_size_ - idx_ != 0) return true;

    // Try to read more bytes
    buffer_size_ = socket_.Recv(buffer_, buffer_max_);
    idx_ = 0;

    return (buffer_size_ != 0);
}

void HTTPSocket::SendRequest(HTTPRequest& request) {
    std::string buff = request.fmt();
    socket_.Send(buff.c_str(), buff.size() - 1); // Avoid '\0'
}

void HTTPSocket::SendResponse(HTTPResponse& response) {
    std::string buff = response.fmt();
    socket_.Send(buff.c_str(), buff.size() - 1); // Avoid '\0'
}

void HTTPSocket::FetchNextRequest(HTTPRequest& request) {
    // Parse the start line
    request.start_line_.method_ = ParseHTTPMethod();
    request.start_line_.request_target_ = ParseRequestTarget();
    request.start_line_.version_ = ParseHTTPVersion();

    // Parse the headers
    std::pair<std::string, std::string> curr_header = ParseNextHeader();
    while (curr_header.first != "" && curr_header.second != "") {
        request.headers_[to_lower(curr_header.first)] = curr_header.second;
        curr_header = ParseNextHeader();
    }

    // Fetch the body
    if (request.headers_.find('content-length') != request.headers_.end())
        request.body_ = ParseBody(atoi(request.headers_['content-length']));
    
}

void HTTPSocket::FetchNextResponse(HTTPResponse& response) {
    // Parse the status line
    response.status_line_.version_ = ParseHTTPVersion();
    response.status_line_.status_code_ = ParseStatus();

    // Parse the headers
    std::pair<std::string, std::string> curr_header = ParseNextHeader();
    while (curr_header.first != "" && curr_header.second != "") {
        response.headers_[to_lower(curr_header.first)] = curr_header.second;
        curr_header = ParseNextHeader();
    }
    
    // Fetch the body
    if (response.headers_.find('content-length') != response.headers_.end())
        response.body_ = ParseBody(atoi(response.headers_['content-length']));
}

HTTPMethod HTTPSocket::ParseHTTPMethod() {
    std::string str = "";

    // Skip whitespace
    while (PeekByte() == ' ' || PeekByte() == '\t' || PeekByte() == '\r' || PeekByte() == '\n') {
        NextByte();
    }

    // Read in the method
    char c;
    while ((c = NextByte()) != ' ')
        str += c;
    
    // Return the method
    if (str == "GET")
        return HTTPMethod::kGet;
    else if (str == "HEAD")
        return HTTPMethod::kHead;
    else if (str == "POST")
        return HTTPMethod::kPost;
    else if (str == "PUT")
        return HTTPMethod::kPut;
    else if (str == "DELETE")
        return HTTPMethod::kDelete;
    else if (str == "CONNECT")
        return HTTPMethod::kConnect;
    else if (str == "OPTIONS")
        return HTTPMethod::kOptions;
    else if (str == "TRACE")
        return HTTPMethod::kTrace;
    else if (str == "PATCH")
        return HTTPMethod::kPatch;
    
    throw HTTPReadError();
}

std::string HTTPSocket::ParseRequestTarget() {
    std::string target = "";
    
    char c;
    while ((c = NextByte()) != ' ')
        target += c;
    
    return target;
}

HTTPVersion HTTPSocket::ParseHTTPVersion() {
    std::string str = "";

    char c;
    while ((c = NextByte()) != '\r')
        str += c;
    
    // Check for new line
    if (NextByte() != '\n')
        throw HTTPReadError();
    
    // Return Version
    if (str == "HTTP/0.9")
        return HTTPVersion::kHTTP_0_9;
    else if (str == "HTTP/1.0")
        return HTTPVersion::kHTTP_1_0;
    else if (str == "HTTP/1.1")
        return HTTPVersion::kHTTP_1_1;
    else if (str == "HTTP/2.0")
        return HTTPVersion::kHTTP_2_0;
    
    throw HTTPReadError();
}

HTTPStatusCode HTTPSocket::ParseStatus() {
    // Parse status code
    std::string str = "";

    char c;
    while ((c = NextByte()) != ' ')
        str += c;
    
    // Convert to int
    uint32 code = atoi(str);
    HTTPStatusCode status_code;
    switch (code)
    {
        case 100:
            status_code = HTTPStatusCode::k100;
            break;
        case 101:
            status_code = HTTPStatusCode::k101;
            break;
        case 102:
            status_code = HTTPStatusCode::k102;
            break;
        case 103:
            status_code = HTTPStatusCode::k103;
            break;
        case 200:
            status_code = HTTPStatusCode::k200;
            break;
        case 201:
            status_code = HTTPStatusCode::k201;
            break;
        case 202:
            status_code = HTTPStatusCode::k202;
            break;
        case 203:
            status_code = HTTPStatusCode::k203;
            break;
        case 204:
            status_code = HTTPStatusCode::k204;
            break;
        case 205:
            status_code = HTTPStatusCode::k205;
            break;
        case 206:
            status_code = HTTPStatusCode::k206;
            break;
        case 207:
            status_code = HTTPStatusCode::k207;
            break;
        case 208:
            status_code = HTTPStatusCode::k208;
            break;
        case 226:
            status_code = HTTPStatusCode::k226;
            break;
        case 300:
            status_code = HTTPStatusCode::k300;
            break;
        case 301:
            status_code = HTTPStatusCode::k301;
            break;
        case 302:
            status_code = HTTPStatusCode::k302;
            break;
        case 303:
            status_code = HTTPStatusCode::k303;
            break;
        case 304:
            status_code = HTTPStatusCode::k304;
            break;
        case 307:
            status_code = HTTPStatusCode::k307;
            break;
        case 308:
            status_code = HTTPStatusCode::k308;
            break;
        case 400:
            status_code = HTTPStatusCode::k400;
            break;
        case 401:
            status_code = HTTPStatusCode::k401;
            break;
        case 402:
            status_code = HTTPStatusCode::k402;
            break;
        case 403:
            status_code = HTTPStatusCode::k403;
            break;
        case 404:
            status_code = HTTPStatusCode::k404;
            break;
        case 405:
            status_code = HTTPStatusCode::k405;
            break;
        case 406:
            status_code = HTTPStatusCode::k406;
            break;
        case 407:
            status_code = HTTPStatusCode::k407;
            break;
        case 408:
            status_code = HTTPStatusCode::k408;
            break;
        case 409:
            status_code = HTTPStatusCode::k409;
            break;
        case 410:
            status_code = HTTPStatusCode::k410;
            break;
        case 411:
            status_code = HTTPStatusCode::k411;
            break;
        case 412:
            status_code = HTTPStatusCode::k412;
            break;
        case 413:
            status_code = HTTPStatusCode::k413;
            break;
        case 414:
            status_code = HTTPStatusCode::k414;
            break;
        case 415:
            status_code = HTTPStatusCode::k415;
            break;
        case 416:
            status_code = HTTPStatusCode::k416;
            break;
        case 417:
            status_code = HTTPStatusCode::k417;
            break;
        case 418:
            status_code = HTTPStatusCode::k418;
            break;
        case 421:
            status_code = HTTPStatusCode::k421;
            break;
        case 422:
            status_code = HTTPStatusCode::k422;
            break;
        case 423:
            status_code = HTTPStatusCode::k423;
            break;
        case 424:
            status_code = HTTPStatusCode::k424;
            break;
        case 426:
            status_code = HTTPStatusCode::k426;
            break;
        case 428:
            status_code = HTTPStatusCode::k428;
            break;
        case 429:
            status_code = HTTPStatusCode::k429;
            break;
        case 431:
            status_code = HTTPStatusCode::k431;
            break;
        case 451:
            status_code = HTTPStatusCode::k451;
            break;
        case 500:
            status_code = HTTPStatusCode::k500;
            break;
        case 501:
            status_code = HTTPStatusCode::k501;
            break;
        case 502:
            status_code = HTTPStatusCode::k502;
            break;
        case 503:
            status_code = HTTPStatusCode::k503;
            break;
        case 504:
            status_code = HTTPStatusCode::k504;
            break;
        case 505:
            status_code = HTTPStatusCode::k505;
            break;
        case 506:
            status_code = HTTPStatusCode::k506;
            break;
        case 507:
            status_code = HTTPStatusCode::k507;
            break;
        case 508:
            status_code = HTTPStatusCode::k508;
            break;
        case 510:
            status_code = HTTPStatusCode::k510;
            break;
        case 511:
            status_code = HTTPStatusCode::k511;
            break;
        default:
            throw HTTPReadError();
            break;
    }

    // Read in the actualy message
    while(NextByte() != '\r')

    // Check the CRLF
    if (NextByte() != '\n')
        throw HTTPReadError();

    return status_code;
}

std::pair<std::string, std::string> HTTPSocket::ParseNextHeader() {
    // Parse the header name
    std::pair<std::string, std::string> ret_val("", "");

    // Check that it is not a blank line
    if (PeekByte() == '\r') {
        NextByte();
        if (NextByte() != '\n')
            return ret_val;
        else 
            throw HTTPReadError();
    }

    char c;
    while ((c = NextByte()) != ':')
        ret_val.first += c;
    
    // Skip any spaces
    while (PeekByte() == ' ')
        NextByte();
    
    // Read in the header value
    while((c = NextByte()) != '\r')
        ret_val.second += c;
    
    // Read in the CRLF
    if (NextByte() != '\n')
        throw HTTPReadError();
    
    return ret_val;
}

std::string HTTPSocket::ParseBody(int32 body_size) {
    char* buffer = new char[body_size + 1];

    if (NextBytes(buffer, body_size) != body_size)
        throw HTTPReadError();
    buffer[body_size] = '\0';
    
    std::string str(buffer);

    delete[] buffer;

    return str;
}
