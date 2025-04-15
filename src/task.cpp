#include "task.h"

namespace task {
    void init(size_t numThreads) {
        ThreadPool::getInstance(numThreads);
    }
}
