
//
// Created by Robert on 2025-10-19.
//

#pragma once
#include "core/CopyStage.h"
#include "logging/LoggerFactory.h"
#include <vector>

namespace bpmfinder::dsp::time_domain_onset_detection
{
    class BpmCalculationStage : public core::CopyStage<TimeDomainOnsetDetectionResult, TimeDomainOnsetDetectionResult>
    {
    public:
        explicit BpmCalculationStage()
            : logger_(logging::LoggerFactory::GetLogger("BpmCalculationStage"))
        {
            logger_->info("BpmCalculationStage initialized");
        }

    protected:
        void Process(TimeDomainOnsetDetectionResult data) override
        {
            // 4. Convert interval from "onset buffer indices" to seconds: each index in onsetBuffer represents one chunk
            const float chunksPerSecond = static_cast<float>(data.sampleRate) / static_cast<float>(data.chunkSize);

            if (!data.dominantInterval.has_value())
            {
                data.bpm = currentBpm_;
                this->Notify(data);
                return;
            }

            // 5. Convert to BPM: BPM = 60 / period_in_seconds
            if (const float intervalInSeconds = data.dominantInterval.value() / chunksPerSecond; intervalInSeconds >
                0.0f)
            {
                float bpm = 60.0f / intervalInSeconds;

                if (bpm == currentBpm_)
                {
                    return;
                }

                currentBpm_ = bpm;
                data.bpm = bpm;

                logger_->warn("BPM: {:.1f}", bpm);

                this->Notify(data);
            }
        }

    private:
        float currentBpm_ = 0.0f;

        std::shared_ptr<spdlog::logger> logger_;
    };
}
