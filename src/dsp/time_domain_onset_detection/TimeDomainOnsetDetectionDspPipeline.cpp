//
// Created by Robert on 2025-10-06.
//

#include "TimeDomainOnsetDetectionDspPipeline.h"
#include <iostream>
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
        energySink("energy.bin"),
        onsetSink("onset.bin")
    {
        // In the ctor we only assemble the dsp chain, start reading audio data and processing it via Start()
        if (!source.Initialize())
        {
            std::cerr << "Failed to init WASAPI source" << std::endl;
            return;
        }

        source.Subscribe(&sink); // Write raw input data to file
        source.Subscribe(&bandPassFilterStage); // Pass raw input data to bandpass filter stage
        bandPassFilterStage.Subscribe(&bandPassSink); // Write bandpass filtered data to file
        bandPassFilterStage.Subscribe(&energyCalculationStage); // Pass bandpass filtered data to energy calc stage
        energyCalculationStage.Subscribe(&energySink); // Write energy data to file
        energyCalculationStage.Subscribe(&onsetDetectionStage); // Pass energy data to onset detection stage
        onsetDetectionStage.Subscribe(&onsetSink); // Write onset data to file
    }

    void TimeDomainOnsetDetectionDspPipeline::Start()
    {
        // Start the pipeline's worker threads BEFORE starting the audio source
        sink.Start();
        bandPassFilterStage.Start();
        bandPassSink.Start();
        energyCalculationStage.Start();
        energySink.Start();
        onsetDetectionStage.Start();
        onsetSink.Start();

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

        sink.StopAndDrain();
        bandPassSink.StopAndDrain();
        energySink.StopAndDrain();

        // Print statistics
        std::cout << "\n=== Pipeline Statistics ===" << std::endl;
        std::cout << "Sink: " << sink.GetProcessedCount() << "/" << sink.GetQueuedCount() << std::endl;
        std::cout << "BandPassFilterStage: " << bandPassFilterStage.GetProcessedCount() << "/"
            << bandPassFilterStage.GetQueuedCount() << std::endl;
        std::cout << "BandPassSink: " << bandPassSink.GetProcessedCount() << "/" << bandPassSink.GetQueuedCount() <<
            std::endl;
        std::cout << "EnergyCalculationStage: " << energyCalculationStage.GetProcessedCount() << "/"
            << energyCalculationStage.GetQueuedCount() << std::endl;
        std::cout << "EnergySink: " << energySink.GetProcessedCount() << "/" << energySink.GetQueuedCount() <<
            std::endl;
        std::cout << "OnsetDetectionStage: " << onsetDetectionStage.GetProcessedCount() << "/"
            << onsetDetectionStage.GetQueuedCount() << std::endl;
        std::cout << "OnsetSink: " << onsetSink.GetProcessedCount() << "/"
            << onsetSink.GetQueuedCount() << std::endl;

        std::cout << "Waveform entries written: " << sink.GetWrittenCount() << "" << std::endl;
        std::cout << "Bandpass entries written: " << bandPassSink.GetWrittenCount() << "" << std::endl;
        std::cout << "Energy entries written: " << energySink.GetWrittenCount() << "" << std::endl;
        std::cout << "Onset entries written: " << onsetSink.GetWrittenCount() << std::endl;
        std::cout << "===========================\n" << std::endl;
    }
}
