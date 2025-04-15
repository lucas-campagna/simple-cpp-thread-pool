#include "impl/thread_pool.h"
#include <thread>

namespace task {
    void init(size_t numThreads = std::thread::hardware_concurrency());
    
    template<class F, class... Args>
    auto run(F&& f, Args&&... args) {
        return ThreadPool::getInstance().enqueue(std::forward<F>(f), std::forward<Args>(args)...);
    }
}