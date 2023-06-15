#ifndef STUB_THREADING_THREAD_POOL_H
#define STUB_THREADING_THREAD_POOL_H

// Windows includes
#include <Windows.h>

// STUB includes
#include "PrimitiveType/primitive_type.h"

namespace stub {
namespace threading {

// Defines
#define DEFAULT_THREAD_NUM 8

// TODO: Look up how to make an effective singleton class

class ThreadPool {
public:
    // Constructors/Destructors
    ThreadPool(primitive_type::uint16 num_threads);
    ~ThreadPool();

    // Submission Functions
    void SubmitSocket(SOCKET socket);

    // Management Functions
    primitive_type::uint8 GetNumThreads();
    void SetNumThreads(primitive_type::uint8 num_threads);
    void Terminate();

private:
    // Retrieval Functions
    SOCKET FetchSocket();

};

};
};

#endif