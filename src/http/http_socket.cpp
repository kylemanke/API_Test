#include <cstring>
#include <string>

#include "exception/exception.h"
#include "types.h"
#include "stub_defines.h"

#include "http_socket.h"

#define BUFFER_SIZE 2048

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

void HTTPSocket::SendRequest(HTTPRequest *request) {
    socket_.Send(request->fmt().c_str(), )
}

