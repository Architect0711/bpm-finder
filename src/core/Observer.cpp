//
// Created by Robert on 2025-09-17.
//

#include "Observer.h"

namespace bpmfinder::core
{
    template <typename DataType>
    void Observer<DataType>::PushData(const DataType& data)
    {
        {
            std::unique_lock lock(mtx_);
            queue_.push(data);
        }
        cv_.notify_one();
    }
}
