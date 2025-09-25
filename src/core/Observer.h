//
// Created by Robert on 2025-09-17.
//

#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

namespace bpmfinder::core
{
    // CRTP Style Observer - Only pushes data to a queue, processing has to be done by other means, e.g. by implementing a WorkerThread
    template <typename DataType>
    class Observer
    {
    private:
        std::mutex mtx_;
        std::condition_variable cv_;
        std::queue<DataType> queue_;

    public:
        void PushData(const DataType& data)
        {
            {
                std::unique_lock lock(mtx_);
                queue_.push(data);
            }
            cv_.notify_one();
        }
    };
}
