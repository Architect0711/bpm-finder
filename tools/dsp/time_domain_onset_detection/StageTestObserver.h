//
// Created by Robert on 2025-10-22.
//

#pragma once

#include <vector>

#include "core/CopyObserver.h"

namespace bpmfinder::tools::dsp::time_domain_onset_detection
{
    // Observer class to capture stage output
    template <typename T>
    class StageTestObserver : public core::CopyObserver<T>
    {
    public:
        std::vector<T> GetData()
        {
            std::vector<T> result;
            // Lock only for the duration of emptying the queue
            {
                while (!this->queue_.empty())
                {
                    result.push_back(this->queue_.front());
                    this->queue_.pop();
                }
            } // Release lock before returning
            return result;
        }

        bool HasResult()
        {
            return !this->queue_.empty();
        }
    };
}

