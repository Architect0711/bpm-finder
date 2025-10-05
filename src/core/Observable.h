//
// Created by Robert on 2025-09-17.
//

#pragma once
#include <vector>
#include <mutex>
#include "Observer.h"

namespace bpmfinder::core
{
    // CRTP Style Observable - Emits objects of the templated data type
    template <typename DataType>
    class Observable
    {
    private:
        std::vector<class Observer<DataType>*> observers_;
        std::mutex mtx_;

    public:
        void Subscribe(Observer<DataType>* obs)
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
                obs->PushData(data); // observer handles thread safety
            }
        }
    };
}
