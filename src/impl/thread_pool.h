#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <future>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>

#if defined(__cpp_lib_is_invocable) && __cpp_lib_is_invocable >= 201703
    // std::result_of is deprecated in C++17 and removed in C++20. Hence, it is
    // replaced with std::invoke_result here.
    template <typename Func, typename... U>
    using FunctionReturnType = std::remove_reference_t<std::remove_cv_t<std::invoke_result_t<Func, U...>>>;
#else
    // Keep ::type here because we still support C++11
    template <typename Func, typename... U>
    using FunctionReturnType = typename std::remove_reference<typename std::remove_cv<typename std::result_of<Func(U...)>::type>::type>::type;
#endif

class ThreadPool {
    private:
        static std::unique_ptr<ThreadPool> instance_;
        static std::once_flag once_flag_;
    
        std::vector<std::thread> workers_;
        std::queue<std::function<void()>> tasks_;
        std::mutex queueMutex_;
        std::condition_variable condition_;
        bool stop_;
    
        // Private constructor to prevent direct instantiation
        ThreadPool(size_t numThreads);
    
        // Prevent copy and move
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;
    
    public:
        static ThreadPool& getInstance(size_t numThreads = std::thread::hardware_concurrency());
    
        ~ThreadPool();
    
        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args) {
            using return_type = FunctionReturnType<F, Args...>;
            auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );
            std::future<return_type> result_future = task_ptr->get_future();
        
            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                if (stop_)
                    throw std::runtime_error("enqueue on stopped ThreadPool");
                tasks_.emplace([task_ptr]() { (*task_ptr)(); }); // Execute the packaged_task
            }
            condition_.notify_one();
            return result_future;
        }
};
    
#endif