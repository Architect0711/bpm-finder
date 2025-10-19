//
// Created by Robert on 2025-10-16.
//

#pragma once
#include "TimeDomainOnsetDetectionResult.h"
#include "audio/IAudioSource.h"
#include "core/CopyStage.h"
#include "dsp/filters/BandPassFilter.h"
#include "logging/LoggerFactory.h"

namespace bpmfinder::dsp::time_domain_onset_detection
{
    class BandPassFilterStage : public core::CopyStage<TimeDomainOnsetDetectionResult, TimeDomainOnsetDetectionResult>
    {
    public:
        explicit BandPassFilterStage(const int lowCutoff, const int highCutoff, const float gain, const int sampleRate)
            :
            filter_(lowCutoff, highCutoff, sampleRate, gain),
            logger_(logging::LoggerFactory::GetLogger("BandPassFilterStage"))
        {
            logger_->debug("BandPassFilterStage initialized");
        }

    protected:
        void Process(TimeDomainOnsetDetectionResult data) override
        {
            logger_->debug("[BandPassFilterStage] Processing chunk {}/{} with index {}", this->GetProcessedCount(),
                           this->GetQueuedCount(), data.chunkIndex);

            filter_.UpdateParameters(data.bandPassLowCutoff, data.bandPassHighCutoff, data.bandPassGain);

            audio::AudioChunk filteredData;
            filteredData.reserve(data.chunkSize);

            for (const float sample : data.rawAudio)
            {
                filteredData.push_back(filter_.Process(sample));
            }

            data.bandPassFiltered = filteredData;

            this->Notify(data);
        }

    private:
        filters::BandPassFilter filter_;
        std::shared_ptr<spdlog::logger> logger_;
    };
}
