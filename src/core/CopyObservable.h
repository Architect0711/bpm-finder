//
// Created by Robert on 2025-09-17.
//

#pragma once
#include <vector>
#include <mutex>
#include "CopyObserver.h"

namespace bpmfinder::core
{
    // CRTP Style CopyObservable - Emits objects of the templated data type
    template <typename DataType>
    class CopyObservable
    {
    private:
        std::vector<class CopyObserver<DataType>*> observers_;
        std::mutex mtx_;

    public:
        void Subscribe(CopyObserver<DataType>* obs)
        {
            std::lock_guard lock(mtx_);
            observers_.push_back(obs);
        }

    protected:
        void Notify(const DataType& data)
        {
            std::lock_guard lock(mtx_);
            for (auto* obs : observers_)
            {
                obs->PushData(data); // Passes by const reference - observer handles thread safety
            }
        }
    };
}
