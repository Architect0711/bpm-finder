//
// Created by Robert on 2025-09-25.
//

#pragma once
#include "Observer.h"
#include <thread>
#include <atomic>
#include <iostream>

namespace bpmfinder::core
{
    template <typename DataType>
    class Sink : public Observer<DataType>
    {
    public:
        Sink() = default;

        virtual ~Sink()
        {
            Stop();
        }

        void Start()
        {
            if (running_) return; // Avoid starting twice

            running_ = true;
            thread_ = std::thread([this]()
            {
                // Lock must be defined outside the loop for waiting
                std::unique_lock<std::mutex> lock(this->mtx_);

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
                        DataType data = std::move(this->queue_.front());
                        this->queue_.pop();

                        // MUST release the lock before processing to avoid blocking
                        // the PushData call on the other thread while we process.
                        lock.unlock();
                        // 2. Process the data (Replace with actual logic)
                        // For example:
                        std::cout << "Processing data: " << data << std::endl;
                        // Simulate work

                        // 3. Re-acquire the lock to check the queue/wait again
                        lock.lock();
                    }
                }
            });
        }

        virtual void Process(DataType data) = 0;

        void Stop()
        {
            running_ = false;
            if (thread_.joinable())
                thread_.join();
        }

        [[nodiscard]] bool IsRunning() const { return running_; }

    private :
        std::thread thread_;
        std::atomic<bool> running_{false};
    };
}
