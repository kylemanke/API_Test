#ifndef STUB_SOCKET_SOCKET_H
#define STUB_SOCKET_SOCKET_H

// Windows Includes
#include <winsock2.h>
#include <ws2tcpip.h>

// STUB includes
#include "PrimitiveType/primitive_type.h"

namespace stub {
namespace socket {

// Universal set-up functions
primitive_type::uint8 InitializeWinsock();
void CloseWinsock();

// Socket class
// Abstracts away the Winsock API
// Sockets are currently only created by the ServerSocket class
class Socket {
public:
    // Constructors
    Socket();
    Socket(SOCKET socket);
    
    // Destructors
    ~Socket();

    // Socket functions
    primitive_type::uint8 Send(const char *buf, primitive_type::uint32 len);
    primitive_type::int32 Recv(char *buf, primitive_type::uint32 len, primitive_type::int32 flags);

    primitive_type::uint8 ShutdownSend();
    primitive_type::uint8 ShutdownRecv();

    void Close();

    // Getters & Setters
    SOCKET getsocket();

private:
    SOCKET socket_;
};

};
};

#endif