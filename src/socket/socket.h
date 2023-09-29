#ifndef STUB_SOCKET_SOCKET_H
#define STUB_SOCKET_SOCKET_H

#include "types.h"

class Socket {
public:
    /// @brief Creates a TCP, IPv4 Socket
    Socket();
    Socket(int32 sockfd)

    /// @brief Currently does nothing
    ~Socket();

    /// @brief Read in buff_size bytes into buffer
    /// @param buffer Destination buffer
    /// @param buff_size Size of destination buffer
    /// @return Number of bytes read
    int32 Recv(char *buffer, uint32 buff_size);
    
    /// @brief Send buff_size bytes
    /// @param buffer Source Buffer
    /// @param buff_size Size of Source Buffer
    /// @return Number of bytes sent
    int32 Send(const char *buffer, uint32 buff_size);
    
    /// @brief Shutdown RDWR of the socket
    void Shutdown();

    /// @brief Used to connect client socket to server
    /// @param host Hostname or IP Address of Server
    /// @param port Designated port of Server
    void Connect(const char *host, const char *port);

    /// @brief Bind server socket to a port
    /// @param port Requested port
    void Bind(uint16 port);

    /// @brief Tell socket to begin listening for connections
    /// @param backlog Number of queued connections to allow
    void Listen(uint32 backlog);
    
    /// @brief Accept the next connection request
    /// @return The new socket
    Socket Accept();

private:
    int32 sockfd_;
};

#endif