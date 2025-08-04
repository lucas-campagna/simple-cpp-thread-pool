# C++ Thread Pool and Task Management

This project provides a simple, efficient, and modern C++ thread pool with a task management interface. It leverages the C++ standard library features like `std::thread`, `std::future`, and `std::packaged_task` to offer a straightforward way to run asynchronous tasks.

## Features

- **Singleton Design:** The `ThreadPool` is implemented as a singleton, ensuring a single, shared pool of worker threads throughout your application. This is especially useful for managing a consistent resource without the overhead of creating and destroying thread pools.
- **Asynchronous Task Execution:** The `task::run` function allows you to enqueue tasks (any callable object like a function or lambda) that will be executed on a separate thread.
- **Future-Based Results:** When you enqueue a task, `task::run` returns a `std::future` object. This allows you to retrieve the result of the task or handle any exceptions that occurred during its execution.
- **Automatic Thread Sizing:** By default, the thread pool is initialized with a number of threads equal to the hardware concurrency of the system, optimizing performance for your specific machine.

## Getting Started

### Prerequisites

You'll need a C++11 or later compiler.

### Usage

1.  **Include the header:** Include the `task.h` header file in your project. This file provides the public interface for the thread pool.

    ```cpp
    #include "task.h"
    ```

2.  **Initialize the thread pool (optional):** You can explicitly initialize the thread pool with a specific number of threads using `task::init()`. If you don't call this, the pool will be created automatically with the number of hardware threads the first time you call `task::run`.

    ```cpp
    // Initialize the thread pool with 4 threads
    task::init(4);
    ```

3.  **Run a task:** Use the `task::run` function to enqueue a task. You can pass any callable object with its arguments.

    -   **Task with no return value:**
        ```cpp
        task::run([]() {
            // Perform some work...
        });
        ```

    -   **Task with a return value:**
        ```cpp
        auto future_result = task::run([]() {
            return "Hello from a thread!";
        });

        // The future can be used to get the result
        std::string result = future_result.get();
        ```

    -   **Task with arguments:**
        ```cpp
        auto future_sum = task::run([](int a, int b) {
            return a + b;
        }, 10, 20);

        int sum = future_sum.get(); // sum will be 30
        ```

### Example

Here's a simple example demonstrating how to use the thread pool to execute multiple tasks concurrently and retrieve their results.

```cpp
#include <iostream>
#include <vector>
#include <future>
#include "task.h"

int main() {
    // The thread pool is automatically initialized with hardware_concurrency threads
    // on the first call to task::run().
    // You could also initialize it manually with task::init(4);

    std::vector<std::future<int>> results;

    for (int i = 0; i < 10; ++i) {
        results.emplace_back(task::run([i] {
            std::cout << "Running task " << i << std::endl;
            // Simulate work
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return i * i;
        }));
    }

    std::cout << "All tasks enqueued. Retrieving results..." << std::endl;

    for (auto& future : results) {
        int value = future.get();
        std::cout << "Task result: " << value << std::endl;
    }

    return 0;
}
```

### How It Works

The core of this project is the ThreadPool class. It manages a fixed number of worker threads that continuously pull tasks from a thread-safe queue.

1. **Singleton Pattern**: A std::call_once and std::once_flag are used to ensure that the ThreadPool is only ever constructed once. This provides a global access point for task management.

2. **Task Queue**: Tasks are stored in a std::queue<std::function<void()>>. Each task is a std::packaged_task, which wraps a function and provides a way to get its result via a std::future.

3. **Synchronization**: A std::mutex and a std::condition_variable are used to safely manage access to the task queue. When a new task is enqueued, the condition variable is notified, waking up a waiting worker thread to process the task. If there are no tasks, worker threads wait on the condition variable.

4. **Graceful Shutdown**: The destructor of the ThreadPool sets a stop flag, notifies all worker threads, and then uses worker.join() to ensure all threads have finished their current tasks and exited gracefully before the pool is destroyed.

### Testing

The project uses the Catch2 testing framework to ensure correctness. The included tests cover various scenarios, including basic task execution, return value retrieval, and exception handling.

To run the tests, compile and run the test_main.cpp file after including the Catch2 header and the project files.

```cpp
#include <catch.hpp>
#include <task.h>

TEST_CASE("Task Shortest Test", "[Task]") {
    // Tests for basic task execution, return values, and exceptions.
}
```
