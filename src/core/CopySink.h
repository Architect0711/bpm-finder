//
// Created by Robert on 2025-09-25.
//

#pragma once
#include "CopyObserver.h"
#include <thread>
#include <atomic>
#include <future>
#include <iostream>

namespace bpmfinder::core
{
    template <typename DataType>
    class CopySink : public CopyObserver<DataType>
    {
    public:
        CopySink() = default;

        virtual ~CopySink()
        {
            Stop();
        }

        void Start()
        {
            if (running_) return; // Avoid starting twice

            running_ = true;
            std::promise<void> thread_ready;
            std::future<void> ready_future = thread_ready.get_future();

            thread_ = std::thread([this, ready_promise = std::move(thread_ready)]() mutable
            {
                // Lock must be defined outside the loop for waiting
                std::unique_lock<std::mutex> lock(this->mtx_);

                // Signal that the thread is ready and waiting
                ready_promise.set_value();

                while (this->running_) // Check if the sink is active
                {
                    // Wait for the condition variable to be notified OR for 'running_' to become false
                    // The lambda predicate prevents spurious wakeups and ensures we wake up
                    // when there is data OR when we are stopping.
                    this->cv_.wait(lock, [this]
                    {
                        return !this->queue_.empty() || !this->running_;
                    });

                    // Check again in case we woke up because 'running_' became false
                    if (!this->running_ && this->queue_.empty())
                    {
                        break;
                    }

                    // Process all data currently in the queue
                    while (!this->queue_.empty())
                    {
                        // 1. Get the data
                        // Move the front element from the queue into local variable 'data'
                        // - `this->queue_.front()` returns a **reference** to the element at the front of the queue
                        // - `std::move(this->queue_.front())` casts that reference to an **rvalue reference** (`DataType&&`)
                        // - The assignment `DataType data = ...` invokes the **move constructor** of `DataType`, which transfers ownership of resources from the queue element to the local variable `data`
                        DataType data = std::move(this->queue_.front());
                        // - After the move, the queue element is in a "valid but unspecified state" (typically empty)
                        // - - `queue_.pop()` removes the now-moved-from element from the queue
                        this->queue_.pop();

                        // MUST release the lock before processing to avoid blocking
                        // the PushData call on the other thread while we process.
                        lock.unlock();

                        // 2. Process the data by calling the virtual method
                        // - Even though we already moved `data` from the queue, the local variable itself is an **lvalue** (it has a name and persists in scope).
                        // When you pass `data` to `Process()`
                        // - **Without `std::move`**: The parameter `DataType data` in `Process()` would **copy** from the local variable
                        // - **With `std::move`**: The parameter in `Process()` will  **move** from the local variable, avoiding a copy
                        Process(std::move(data));
                        ++this->processed_count_;

                        // 3. Re-acquire the lock to check the queue/wait again
                        lock.lock();
                    }
                }
            });

            // Wait until the worker thread is ready
            ready_future.wait();
        }

        virtual void Process(DataType data) = 0;

        void Stop()
        {
            running_ = false;
            this->cv_.notify_one(); // ← CRITICAL: Wake up the worker thread!
            if (thread_.joinable())
                thread_.join();
        }

        void StopAndDrain(std::chrono::milliseconds timeout = std::chrono::milliseconds(5000))
        {
            auto start = std::chrono::steady_clock::now();

            // Wait for queue to be empty or timeout
            while (!IsQueueEmpty())
            {
                if (std::chrono::steady_clock::now() - start > timeout)
                {
                    std::cerr << "Warning: StopAndDrain timeout reached, queue not empty!" << std::endl;
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            Stop();
        }

        template <typename PredecessorType>
        void StopAfterPredecessor(PredecessorType& predecessor,
                                  std::chrono::milliseconds timeout = std::chrono::milliseconds(5000))
        {
            auto start = std::chrono::steady_clock::now();

            // Wait for predecessor to finish processing (queue empty and not running)
            while (predecessor.IsRunning() || !predecessor.IsQueueEmpty())
            {
                if (std::chrono::steady_clock::now() - start > timeout)
                {
                    std::cerr << "Warning: StopAfterPredecessor timeout reached!" << std::endl;
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            Stop();
        }

        [[nodiscard]] bool IsRunning() const { return running_; }

        [[nodiscard]] bool IsQueueEmpty()
        {
            std::lock_guard lock(this->mtx_);
            return this->queue_.empty();
        }

    private :
        std::thread thread_;
        std::atomic<bool> running_{false};
    };
}
