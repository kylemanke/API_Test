#ifndef STUB_UTILS_SOCKET_DEQUE
#define STUB_UTILS_SOCKET_DEQUE

#include "PrimitiveType/primitive_type.h"
#include "WinSock2.h"

namespace stub {
namespace utils {

struct Node {
    SOCKET socket_;
    Node *next_;
};

class SocketDeque {
public:
    SocketDeque() : front_(nullptr), back_(nullptr), size_(0) {}
    
    ~SocketDeque() {
        Node *curr = front_;
        while (curr != nullptr) {
            Node *temp = curr;
            curr = curr->next_;
            delete temp;
        }
    }

    void Push(SOCKET socket) {
        Node *new_node = new Node();
        new_node->socket_ = socket;
        new_node->next_ = nullptr;

        if (size_ == 0) {
            front_ = new_node;
            back_ = new_node;
        } else {
            back_->next_ = new_node;
            back_ = new_node;
        }
        ++size_;
    }

    SOCKET Front() {
        if (size_ == 0) return INVALID_SOCKET;
        return front_->socket_;
    }

    void Pop() {
        Node *prev_front;
        if (size_ == 0) return;
        else if (size_ == 1) {
            delete front_;
            front_ = nullptr;
            back_ = nullptr;
        } else {
            prev_front = front_;
            front_ = front_->next_;
            delete prev_front;
        }
        --size_;
    }

    primitive_type::uint32 size() {
        return size_;
    }

private:
    Node *front_;
    Node *back_;
    primitive_type::uint32 size_;    
};

};
};

#endif