#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

#include <cstring>

#include "exception/exception.h"
#include "socket.h"

Socket::Socket() {
    sockfd_ = -1;

    if ((sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw SocketException(errno);
}

Socket::Socket(int32 sockfd) {
    sockfd_ = sockfd;
}

Socket::Socket(Socket &sock) {
    sockfd_ = sock.sockfd_;
}

Socket::~Socket() {
    // Pass
}

bool Socket::Valid() {
    return !(sockfd_ == -1);
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
    sockfd_ = -1;
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
    struct sockaddr_in serv_addr;
    
    // Set up the serv_addr
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind to the socket
    if (bind(sockfd_, (sockaddr*) &serv_addr, sizeof(serv_addr)) != 0) 
        throw SocketException(errno);
}

void Socket::Listen(uint32 backlog) {
    if (listen(sockfd_, backlog) == -1)
        throw SocketException(errno);
}

Socket Socket::Accept() {
    uint32 ret_val;
    if ((ret_val = accept(sockfd_, nullptr, nullptr)) == -1)
        throw SocketException(errno);
    return Socket(ret_val);
}
