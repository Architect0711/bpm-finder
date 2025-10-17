//
// Created by Robert on 2025-10-06.
//

#pragma once

#include "BandPassFilterStage.h"
#include "EnergyCalculationStage.h"
#include "audio/WasapiAudioSource.h"
#include "audio/AudioBinFileSink.h"
#include "audio/EnergyBinFileSink.h"

namespace bpmfinder::dsp::time_domain_onset_detection
{
    class TimeDomainOnsetDetectionDspPipeline
    {
    public:
        explicit TimeDomainOnsetDetectionDspPipeline(int chunkSize = 1024, int sampleRate = 48000,
                                                     int bandPassLowCutoff = 100, int bandPassHighCutoff = 10000);
        int chunkSize;
        int sampleRate;
        int bandPassLowCutoff;
        int bandPassHighCutoff;

        void Start();
        void Stop();

    private:
        audio::WasapiAudioSource source;
        BandPassFilterStage bandPassFilterStage;
        EnergyCalculationStage energyCalculationStage;

        audio::AudioBinFileSink sink;
        audio::AudioBinFileSink bandPassSink;
        audio::EnergyBinFileSink energySink;
    };
}
