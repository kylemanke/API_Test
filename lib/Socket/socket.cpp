// C++ includes
#include <stdio.h>

// STUB Includes

// Local Includes
#include "socket.h"

namespace stub {
namespace socket {

// Universal set-up functions
// InitializeWinsock -> int
// Will return 0 on success, otherwise fail
primitive_type::uint8 InitializeWinsock() {
    WSAData wsa_data;
    primitive_type::int32 result;
    
    result = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return 1;
    }

    return 0;
}

void CloseWinsock() {
    WSACleanup();
}

// Socket class
Socket::Socket() : socket_(INVALID_SOCKET) {}
Socket::Socket(SOCKET socket) : socket_(socket) {}

Socket::~Socket() {}

// Send -> uint_8
// Return 0 on success, 1 otherwise
primitive_type::uint8 Socket::Send(const char *buf, primitive_type::uint32 len) {
    primitive_type::int32 send_result;
    send_result = send(socket_, buf, len, 0);
    if (send_result == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        return 1;
    }
    return 0;
}

// Recv -> int_32
// Will return number of bytes read or -1 if failed
primitive_type::int32 Socket::Recv(char *buf, primitive_type::uint32 len, primitive_type::int32 flags) {
    primitive_type::int32 recv_result;
    recv_result = recv(socket_, buf, len, flags);
    if (recv_result == SOCKET_ERROR) {
        printf("recv failed: %d\n", WSAGetLastError());
        return -1;
    }
    return recv_result;
}

// ShutdownSend, ShutdownRecv -> uint_8
// 0 on success, 1 on failure
primitive_type::uint8 Socket::ShutdownSend() {
    if (shutdown(socket_, SD_SEND) == SOCKET_ERROR) {
        printf("shutdown send failed: %d\n", WSAGetLastError());
        return 1;
    }
    return 0;
}

primitive_type::uint8 Socket::ShutdownRecv() {
    if (shutdown(socket_, SD_RECEIVE) == SOCKET_ERROR) {
        printf("shutdown recv failed: %d\n", WSAGetLastError());
        return 1;
    }
    return 0;
}

void Socket::Close() {
    closesocket(socket_);
    socket_ = INVALID_SOCKET;
}

SOCKET Socket::getsocket() {
    return socket_;
}

};
};

