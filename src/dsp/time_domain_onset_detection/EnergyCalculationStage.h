//
// Created by Robert on 2025-10-16.
//

#pragma once
#include "audio/IAudioSource.h"
#include "core/CopyStage.h"

namespace bpmfinder::dsp::time_domain_onset_detection
{
    class EnergyCalculationStage : public core::CopyStage<audio::AudioChunk, float>
    {
    public:
        explicit EnergyCalculationStage()
        {
        }

    protected:
        void Process(audio::AudioChunk data) override
        {
            std::cout << "[EnergyCalculationStage] Processing chunk " << this->GetProcessedCount()
                << "/" << this->GetQueuedCount() << std::endl;

            // Calculate energy: E = sum(s[n]^2) for n=0 to N-1
            float energy = 0.0f;
            for (const float sample : data)
            {
                energy += sample * sample;
            }

            this->Notify(energy);
        }
    };
}
