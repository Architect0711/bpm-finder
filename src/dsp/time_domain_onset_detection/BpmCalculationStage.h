
//
// Created by Robert on 2025-10-19.
//

#pragma once
#include "core/CopyStage.h"
#include "logging/LoggerFactory.h"
#include <vector>

namespace bpmfinder::dsp::time_domain_onset_detection
{
    class BpmCalculationStage : public core::CopyStage<float, float>
    {
    public:
        explicit BpmCalculationStage(const int sampleRate, const int chunkSize)
            : sampleRate_(sampleRate),
              chunkSize_(chunkSize),
              logger_(logging::LoggerFactory::GetLogger("BpmCalculationStage"))
        {
            logger_->info("BpmCalculationStage initialized - Sample Rate: {}s, Chunk Size: {}", sampleRate_,
                          chunkSize_);
        }

    protected:
        void Process(const float dominantIntervalInSamples) override
        {
            // 4. Convert interval from "onset buffer indices" to seconds: each index in onsetBuffer represents one chunk
            const float chunksPerSecond = static_cast<float>(sampleRate_) / static_cast<float>(chunkSize_);

            // 5. Convert to BPM: BPM = 60 / period_in_seconds
            if (const float intervalInSeconds = dominantIntervalInSamples / chunksPerSecond; intervalInSeconds > 0.0f)
            {
                float bpm = 60.0f / intervalInSeconds;

                if (bpm == currentBpm_)
                {
                    return;
                }

                currentBpm_ = bpm;

                logger_->warn("BPM: {:.1f}", bpm);

                ++calculationCount_;

                this->Notify(bpm);
            }
        }

    private:
        int sampleRate_;
        int chunkSize_;
        size_t calculationCount_ = 0;
        float currentBpm_ = 0.0f;

        std::shared_ptr<spdlog::logger> logger_;
    };
}
