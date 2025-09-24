//
// Created by Robert on 2025-09-17.
//

#include "Observable.h"
#include "Observer.h"

namespace bpmfinder::core
{
    template <typename DataType>
    void Observable<DataType>::Subscribe(Observer<DataType>* obs)
    {
        std::lock_guard lock(mtx_);
        observers_.push_back(obs);
    }

    template <typename DataType>
    void Observable<DataType>::Notify(const DataType& data)
    {
        std::lock_guard lock(mtx_);
        for (auto* obs : observers_)
        {
            obs->PushData(data); // observer handles thread safety
        }
    }
}
