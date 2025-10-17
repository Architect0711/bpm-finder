//
// Created by Robert on 2025-09-17.
//

#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace bpmfinder::core
{
    // CRTP Style CopyObserver - Only pushes data to a queue, processing has to be done by other means, e.g. by implementing a WorkerThread
    template <typename DataType>
    class CopyObserver
    {
    protected:
        std::mutex mtx_;
        std::condition_variable cv_;
        std::queue<DataType> queue_;
        std::atomic<size_t> queued_count_{0}; // Total items pushed to queue
        std::atomic<size_t> processed_count_{0}; // Total items processed

    public:
        void PushData(const DataType& data)
        {
            // Lock only for the duration of putting new data into the queue
            {
                std::unique_lock lock(mtx_);
                queue_.push(data); // ⚠️ COPIES data into the queue!
                queued_count_++;
            } // Release lock before notifying
            cv_.notify_one();
        }

        [[nodiscard]] size_t GetQueuedCount() const { return queued_count_.load(); }
        [[nodiscard]] size_t GetProcessedCount() const { return processed_count_.load(); }
    };
}
