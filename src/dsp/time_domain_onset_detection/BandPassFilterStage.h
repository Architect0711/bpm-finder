//
// Created by Robert on 2025-10-16.
//

#pragma once
#include "audio/IAudioSource.h"
#include "core/CopyStage.h"
#include "dsp/filters/BandPassFilter.h"

namespace bpmfinder::dsp::time_domain_onset_detection
{
    class BandPassFilterStage : public core::CopyStage<audio::AudioChunk, audio::AudioChunk>
    {
    public:
        explicit BandPassFilterStage(const float lowCutoff, const float highCutoff, const float sampleRate)
            :
            filter_(lowCutoff, highCutoff, sampleRate, 1.0f)
        {
        }

    protected:
        void Process(audio::AudioChunk data) override
        {
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
    };
}
