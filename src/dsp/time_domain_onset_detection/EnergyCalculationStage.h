//
// Created by Robert on 2025-10-16.
//

#pragma once
#include "audio/IAudioSource.h"
#include "core/CopyStage.h"

namespace bpmfinder::dsp::time_domain_onset_detection
{
    class EnergyCalculationStage : public core::CopyStage<
            TimeDomainOnsetDetectionResult, TimeDomainOnsetDetectionResult>
    {
    public:
        explicit EnergyCalculationStage() : logger_(logging::LoggerFactory::GetLogger("EnergyCalculationStage"))
        {
        }

    protected:
        void Process(TimeDomainOnsetDetectionResult data) override
        {
            logger_->debug("[EnergyCalculationStage] Processing chunk {}/{} with index {}", this->GetProcessedCount(),
                           this->GetQueuedCount(), data.chunkIndex);

            // Calculate energy: E = sum(s[n]^2) for n=0 to N-1
            float energy = 0.0f;
            for (const float sample : data.bandPassFiltered)
            {
                energy += sample * sample;
            }

            data.energy = energy;

            this->Notify(data);
        }

    private:
        std::shared_ptr<spdlog::logger> logger_;
    };
}
