//
// Created by Robert on 2025-10-16.
//

#pragma once
#include "audio/IAudioSource.h"
#include "core/CopyStage.h"
#include "dsp/filters/BandPassFilter.h"
#include "logging/LoggerFactory.h"

namespace bpmfinder::dsp::time_domain_onset_detection
{
    class BandPassFilterStage : public core::CopyStage<audio::AudioChunk, audio::AudioChunk>
    {
    public:
        explicit BandPassFilterStage(const float lowCutoff, const float highCutoff, const float sampleRate)
            :
            filter_(lowCutoff, highCutoff, sampleRate, 1.0f),
            logger_(logging::LoggerFactory::GetLogger("BandPassFilterStage"))
        {
            logger_->debug("BandPassFilterStage initialized");
        }

    protected:
        void Process(audio::AudioChunk data) override
        {
            logger_->debug("[BandPassFilterStage] Processing chunk {}/{}", this->GetProcessedCount(),
                           this->GetQueuedCount());

            audio::AudioChunk filteredData;
            filteredData.reserve(data.size());

            for (const float sample : data)
            {
                filteredData.push_back(filter_.Process(sample));
            }

            this->Notify(filteredData);
        }

    private:
        filters::BandPassFilter filter_;
        std::shared_ptr<spdlog::logger> logger_;
    };
}
