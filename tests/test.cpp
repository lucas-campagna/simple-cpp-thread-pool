#include <catch.hpp>
#include <task.h>
#include <string>
#include <vector>
#include <future>

TEST_CASE("Task Shortest Test", "[Task]") {
    // (OPTIONAL) Get the singleton instance (defaulting to the number of hardware threads)
    task::init();

    task::run([] {
        CHECK(true);
    });

    {
        auto result = task::run([] {
            CHECK(true);
            return "Task completed!";
        });
        result.wait();
        CHECK(result.valid());
        CHECK(result.get() == "Task completed!");
    }

    {
        auto result = task::run([] {
            throw std::runtime_error("Task failed!");
            CHECK(false);
        });
        result.wait();
        CHECK(result.valid());
        CHECK_THROWS_AS(result.get(), std::runtime_error);
    }
}

TEST_CASE("Task Basic Test", "[Task]") {
    {
        std::vector<std::future<std::string>> results;
        for (int i = 0; i < 5; ++i) {
            results.emplace_back(
                task::run([i] {
                    CHECK(true);
                    return "Task " + std::to_string(i) + " completed!";
                })
            );
        }

        for (auto& result : results) {
            result.wait();
            CHECK(result.valid());
        }
    }
    {
        std::vector<std::future<int>> results;
        for (int i = 0; i < 5; ++i) {
            results.emplace_back(
                task::run([i] {
                    CHECK(true);
                    return i;
                })
            );
        }

        for (auto& result : results) {
            result.wait();
            CHECK(result.valid());
        }
    }
}