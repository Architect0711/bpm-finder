//
// Created by Robert on 2025-10-22.
//

#pragma once

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <csignal>
#include <vector>

#include "core/CopyObserver.h"
#include "dsp/time_domain_onset_detection/TimeDomainOnsetDetectionResult.h"
#include "nlohmann/json_fwd.hpp"

namespace bpmfinder::tools::dsp::time_domain_onset_detection
{
    // Observer class to capture stage output
    template <typename T>
    class StageTestObserver : public core::CopyObserver<T>
    {
    public:
        std::vector<T> GetData()
        {
            return this->queue_;
        }
    };
}

