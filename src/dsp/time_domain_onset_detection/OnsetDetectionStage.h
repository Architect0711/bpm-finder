//
// Created by Robert on 2025-10-17.
//

#pragma once
#include "core/CopyStage.h"
#include <iostream>
#include <algorithm>

namespace bpmfinder::dsp::time_domain_onset_detection
{
    class OnsetDetectionStage : public core::CopyStage<TimeDomainOnsetDetectionResult, TimeDomainOnsetDetectionResult>
    {
    public:
        OnsetDetectionStage() : previousEnergy_(0.0f),
                                logger_(logging::LoggerFactory::GetLogger("OnsetDetectionStage"))
        {
        }

    protected:
        void Process(TimeDomainOnsetDetectionResult data) override
        {
            logger_->debug("[OnsetDetectionStage] Processing chunk {}/{} with index {}", this->GetProcessedCount(),
                           this->GetQueuedCount(), data.chunkIndex);

            // Calculate onset strength signal (OSS)
            // OSS[k] = max(0, E_current - E_previous)
            const float oss = std::max(0.0f, data.energy - previousEnergy_);
            // Update previous energy for next iteration
            previousEnergy_ = data.energy;
            // Store oss signal in result object
            data.onsetStrength = oss;

            // Notify observers with the onset strength value
            this->Notify(data);
        }

    private:
        float previousEnergy_;
        std::shared_ptr<spdlog::logger> logger_;
    };
}
