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
        void Subscribe(Observer<DataType>* obs);

    protected:
        void Notify(const DataType& data);
    };
}
