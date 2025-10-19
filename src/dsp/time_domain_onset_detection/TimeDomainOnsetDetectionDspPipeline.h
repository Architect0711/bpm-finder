//
// Created by Robert on 2025-10-06.
//

#pragma once

#include "BandPassFilterStage.h"
#include "BpmCalculationStage.h"
#include "DominantIntervalCalculationStage.h"
#include "EnergyCalculationStage.h"
#include "InterOnsetIntervalCalculationStage.h"
#include "OnsetDetectionStage.h"
#include "PeakIndexDetectionStage.h"
#include "PipelineResultInitializationStage.h"
#include "audio/WasapiAudioSource.h"
#include "../../files/bin/AudioBinFileSink.h"

namespace bpmfinder::dsp::time_domain_onset_detection
{
    class TimeDomainOnsetDetectionDspPipeline
    {
    public:
        explicit TimeDomainOnsetDetectionDspPipeline(int chunkSize, int sampleRate,
                                                     int bandPassLowCutoff, int bandPassHighCutoff,
                                                     float bandPassGain);
        int chunkSize;
        int sampleRate;
        int bandPassLowCutoff;
        int bandPassHighCutoff;
        float bandPassGain;

        void Start();
        void Stop();

    private:
        audio::WasapiAudioSource source;
        PipelineResultInitializationStage initializationStage;
        BandPassFilterStage bandPassFilterStage;
        EnergyCalculationStage energyCalculationStage;
        OnsetDetectionStage onsetDetectionStage;
        PeakIndexDetectionStage peakIndexDetectionStage;
        InterOnsetIntervalCalculationStage interOnsetIntervalCalculationStage;
        DominantIntervalCalculationStage dominantIntervalCalculationStage;
        BpmCalculationStage bpmCalculationStage;

        files::bin::AudioBinFileSink sink;

        std::shared_ptr<spdlog::logger> logger_;
    };
}
