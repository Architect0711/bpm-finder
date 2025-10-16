//
// Created by Robert on 2025-10-06.
//

#include "TimeDomainOnsetDetectionDspPipeline.h"
#include <iostream>
#include "audio/AudioBinFileSink.h"
#include "audio/WasapiAudioSource.h"

namespace bpmfinder::dsp::time_domain_onset_detection
{
    TimeDomainOnsetDetectionDspPipeline::TimeDomainOnsetDetectionDspPipeline(
        const int chunkSize, const int sampleRate, const int bandPassLowCutoff,
        const int bandPassHighCutoff)
        :
        chunkSize(chunkSize),
        sampleRate(sampleRate),
        bandPassLowCutoff(bandPassLowCutoff),
        bandPassHighCutoff(bandPassHighCutoff),
        bandPassFilterStage(bandPassLowCutoff, bandPassHighCutoff, sampleRate),
        source(chunkSize),
        sink("waveform.bin"),
        bandPassSink("bandpass.bin")
    {
        // In the ctor we only assemble the dsp chain, start reading audio data and processing it via Start()
        if (!source.Initialize())
        {
            std::cerr << "Failed to init WASAPI source" << std::endl;
            return;
        }

        source.Subscribe(&sink);
        source.Subscribe(&bandPassFilterStage);
        bandPassFilterStage.Subscribe(&bandPassSink);
    }

    void TimeDomainOnsetDetectionDspPipeline::Start()
    {
        // Start the sink's worker thread BEFORE starting the audio source
        sink.Start();
        bandPassFilterStage.Start();
        bandPassSink.Start();

        source.Start();
    }

    void TimeDomainOnsetDetectionDspPipeline::Stop()
    {
        // First stop the data flow from the audio source and then the rest of the pipeline
        source.Stop();

        sink.Stop();
        bandPassFilterStage.Stop();
        bandPassSink.Stop();
    }
}
