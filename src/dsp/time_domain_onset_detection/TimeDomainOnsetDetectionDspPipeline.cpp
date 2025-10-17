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
        bandPassSink("bandpass.bin"),
        energySink("energy.bin")
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
        bandPassFilterStage.Subscribe(&energyCalculationStage);
        energyCalculationStage.Subscribe(&energySink);
    }

    void TimeDomainOnsetDetectionDspPipeline::Start()
    {
        // Start the sink's worker thread BEFORE starting the audio source
        sink.Start();
        bandPassFilterStage.Start();
        bandPassSink.Start();
        energyCalculationStage.Start();
        energySink.Start();

        source.Start();
    }

    void TimeDomainOnsetDetectionDspPipeline::Stop()
    {
        // First stop the data flow from the audio source and then the rest of the pipeline
        source.Stop();

        // Then stop the pipeline stages in REVERSE order to allow them to drain their queues
        // Stop sinks last to ensure all processed data is written
        bandPassFilterStage.StopAndDrain();
        energyCalculationStage.StopAndDrain();

        sink.Stop();
        bandPassSink.Stop();
        energySink.Stop();

        // Print statistics
        std::cout << "\n=== Pipeline Statistics ===" << std::endl;
        std::cout << "Sink: " << sink.GetProcessedCount() << "/" << sink.GetQueuedCount() << std::endl;
        std::cout << "BandPassFilterStage: " << bandPassFilterStage.GetProcessedCount() << "/"
            << bandPassFilterStage.GetQueuedCount() << std::endl;
        std::cout << "BandPassSink: " << bandPassSink.GetProcessedCount() << "/" << bandPassSink.GetQueuedCount() <<
            std::endl;
        std::cout << "EnergyCalculationStage: " << energyCalculationStage.GetProcessedCount() << "/"
            << energyCalculationStage.GetQueuedCount() << std::endl;
        std::cout << "EnergySink: " << energySink.GetProcessedCount() << "/"
            << energySink.GetQueuedCount() << std::endl;

        std::cout << "Waveform entries written: " << sink.GetWrittenCount() << "";
        std::cout << "Bandpass entries written: " << bandPassSink.GetWrittenCount() << "";
        std::cout << "Energy entries written: " << energySink.GetWrittenCount() << "";
        std::cout << "===========================\n" << std::endl;
    }
}
