//
// Created by Robert on 2025-09-17.
//

#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

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

    public:
        void PushData(const DataType& data)
        {
            // Lock only for the duration of putting new data into the queue
            {
                std::unique_lock lock(mtx_);
                queue_.push(data); // ⚠️ COPIES data into the queue!
            } // Release lock before notifying
            cv_.notify_one();
        }
    };
}
