#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

#include <cstring>

#include "exception.h"
#include "socket.h"

Socket::Socket() {
    sockfd_ = -1;

    if ((sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw SocketException(errno);
}

Socket::Socket(int32 sockfd) {
    sockfd_ = sockfd;
}

Socket::~Socket() {
    // Pass
}

int32 Socket::Recv(char *buffer, uint32 buff_size) {
    int32 ret_val;
    if ((ret_val = read(sockfd_, buffer, buff_size)) == -1)
        throw SocketException(errno);
    return ret_val;
}

int32 Socket::Send(const char *buffer, uint32 buff_size) {
    int32 ret_val;
    if ((ret_val = write(sockfd_, buffer, buff_size)) == -1)
        throw SocketException(errno);
    return ret_val;
}

void Socket::Shutdown() {
    if (shutdown(sockfd_, SHUT_RDWR) == -1)
        throw SocketException(errno);
}

void Socket::Connect(const char *hostname, const char *port) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    // Convert hostname to ip address
    if (getaddrinfo(hostname, port, &hints, &result) != 0)
        throw SocketException(0);
    
    // Try all results
    for (rp = result; rp != nullptr; rp = rp->ai_next) {
        if (connect(sockfd_, rp->ai_addr, rp->ai_addrlen) != -1)
            break;
    }

    freeaddrinfo(result);

    if (rp == nullptr)
        throw SocketException(0);
}

void Socket::Bind(uint16 port) {
    
}


