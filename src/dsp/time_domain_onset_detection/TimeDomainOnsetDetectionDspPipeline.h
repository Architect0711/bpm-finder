//
// Created by Robert on 2025-10-06.
//

#pragma once

#include "BandPassFilterStage.h"
#include "BpmCalculationStage.h"
#include "EnergyCalculationStage.h"
#include "OnsetDetectionStage.h"
#include "audio/WasapiAudioSource.h"
#include "../../files/bin/AudioBinFileSink.h"
#include "../../files/bin/FloatBinFileSink.h"

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
        OnsetDetectionStage onsetDetectionStage;
        BpmCalculationStage bpmCalculationStage;

        files::bin::AudioBinFileSink sink;
        files::bin::AudioBinFileSink bandPassSink;
        files::bin::FloatBinFileSink energySink;
        files::bin::FloatBinFileSink onsetSink;

        std::shared_ptr<spdlog::logger> logger_;
    };
}
