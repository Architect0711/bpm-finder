//
// Created by Robert on 2025-10-17.
//

#pragma once
#include "core/CopyStage.h"
#include <iostream>
#include <algorithm>

namespace bpmfinder::dsp::time_domain_onset_detection
{
    class OnsetDetectionStage : public core::CopyStage<float, float>
    {
    public:
        OnsetDetectionStage() : previousEnergy_(0.0f),
                                logger_(logging::LoggerFactory::GetLogger("OnsetDetectionStage"))
        {
        }

    protected:
        void Process(float currentEnergy) override
        {
            logger_->debug("[OnsetDetectionStage] Processing chunk {}/{}", this->GetProcessedCount(),
                           this->GetQueuedCount());

            // Calculate onset strength signal (OSS)
            // OSS[k] = max(0, E_current - E_previous)
            const float oss = std::max(0.0f, currentEnergy - previousEnergy_);
            // Update previous energy for next iteration
            previousEnergy_ = currentEnergy;

            // Notify observers with the onset strength value
            this->Notify(oss);
        }

    private:
        float previousEnergy_;
        std::shared_ptr<spdlog::logger> logger_;
    };
}
