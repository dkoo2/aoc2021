#pragma once

#include <cassert>
#include <cstddef>
#include <functional>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "absl/functional/any_invocable.h"
#include "absl/synchronization/mutex.h"

namespace common {

class ThreadPool {
   public:
    explicit ThreadPool(int num_threads) {
        threads_.reserve(num_threads);
        num_threads_ = num_threads;
        for (int i = 0; i < num_threads; ++i) {
            threads_.push_back(std::thread(&ThreadPool::WorkLoop, this));
        }
    }

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

    ~ThreadPool() {
        {
            absl::MutexLock l(&mu_);
            for (size_t i = 0; i < threads_.size(); i++) {
                queue_.push(nullptr);
            }
        }
        for (auto &t : threads_) {
            t.join();
        }
    }

    int size() const { return num_threads_; }

    void Schedule(absl::AnyInvocable<void()> func) {
        assert(func != nullptr);
        absl::MutexLock l(&mu_);
        queue_.push(std::move(func));
    }

   private:
    bool WorkAvailable() const { return !queue_.empty(); }

    void WorkLoop() {
        while (true) {
            absl::AnyInvocable<void()> func;
            {
                absl::MutexLock l(&mu_);
                mu_.Await(absl::Condition(this, &ThreadPool::WorkAvailable));
                func = std::move(queue_.front());
                queue_.pop();
            }
            if (func == nullptr) {  // Shutdown signal.
                break;
            }
            func();
        }
    }

    absl::Mutex mu_;
    std::queue<absl::AnyInvocable<void()>> queue_;
    std::vector<std::thread> threads_;
    int num_threads_;
};

}  // namespace common
