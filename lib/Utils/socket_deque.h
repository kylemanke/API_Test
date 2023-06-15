#ifndef STUB_UTILS_SOCKET_DEQUE
#define STUB_UTILS_SOCKET_DEQUE

#include "PrimitiveType/primitive_type.h"
#include <WinSock2.h>
#include <Synchapi.h>

namespace stub {
namespace utils {

// TODO: Change to work with functor objects

// Defines
#define NODE_CACHE_SIZE 5
#define SPIN_COUNT 0x00000400

struct SocketNode {
    SOCKET socket_;
    SocketNode *next_;
};

class SocketDeque {
public:
    SocketDeque();
    ~SocketDeque();

    void Push(SOCKET socket);
    SOCKET Pop();
    primitive_type::uint32 size();

    void Terminate();

private:
    // Deque items
    SocketNode *front_;
    SocketNode *back_;
    primitive_type::uint32 size_;

    // Store unused nodes for efficiency
    SocketNode *node_cache_[NODE_CACHE_SIZE];
    primitive_type::uint8 cache_size_;

    // Synchronization items
    CRITICAL_SECTION critical_section_;
    CONDITION_VARIABLE not_empty_;
    primitive_type::uint8 terminate_;
};

};
};

#endif