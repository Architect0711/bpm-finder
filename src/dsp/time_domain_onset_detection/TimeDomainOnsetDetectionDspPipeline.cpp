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
        onsetSink("onset.bin"),
        bpmCalculationStage(sampleRate, chunkSize),
        logger_(logging::LoggerFactory::GetLogger("TimeDomainOnsetDetectionDspPipeline"))
    {
        // In the ctor we only assemble the dsp chain, start reading audio data and processing it via Start()
        if (!source.Initialize())
        {
            logger_->error("Failed to init WASAPI source");
            return;
        }

        source.Subscribe(&sink); // Write raw input data to file
        source.Subscribe(&bandPassFilterStage); // Pass raw input data to bandpass filter stage
        bandPassFilterStage.Subscribe(&bandPassSink); // Write bandpass filtered data to file
        bandPassFilterStage.Subscribe(&energyCalculationStage); // Pass bandpass filtered data to energy calc stage
        energyCalculationStage.Subscribe(&energySink); // Write energy data to file
        energyCalculationStage.Subscribe(&onsetDetectionStage); // Pass energy data to onset detection stage
        onsetDetectionStage.Subscribe(&onsetSink); // Write onset data to file
        onsetDetectionStage.Subscribe(&bpmCalculationStage); // Pass onset data to bpm calc stage
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
        bpmCalculationStage.Start();

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
        onsetDetectionStage.StopAndDrain();
        bpmCalculationStage.StopAndDrain();

        sink.StopAndDrain();
        bandPassSink.StopAndDrain();
        energySink.StopAndDrain();
        onsetSink.StopAndDrain();

        // Print statistics
        logger_->info("\n");
        logger_->info("=== Pipeline Statistics ===");
        logger_->info("Sink: {}/{}", sink.GetProcessedCount(), sink.GetQueuedCount());
        logger_->info("BandPassFilterStage: {}/{}",
                      bandPassFilterStage.GetProcessedCount(), bandPassFilterStage.GetQueuedCount());
        logger_->info("BandPassSink: {}/{}",
                      bandPassSink.GetProcessedCount(), bandPassSink.GetQueuedCount());
        logger_->info("EnergyCalculationStage: {}/{}",
                      energyCalculationStage.GetProcessedCount(), energyCalculationStage.GetQueuedCount());
        logger_->info("EnergySink: {}/{}",
                      energySink.GetProcessedCount(), energySink.GetQueuedCount());
        logger_->info("OnsetDetectionStage: {}/{}",
                      onsetDetectionStage.GetProcessedCount(), onsetDetectionStage.GetQueuedCount());
        logger_->info("OnsetSink: {}/{}",
                      onsetSink.GetProcessedCount(), onsetSink.GetQueuedCount());
        logger_->info("BpmCalculationStage: {}/{}", bpmCalculationStage.GetProcessedCount(),
                      bpmCalculationStage.GetQueuedCount());
        logger_->info("Waveform entries written: {}", sink.GetWrittenCount());
        logger_->info("Bandpass entries written: {}", bandPassSink.GetWrittenCount());
        logger_->info("Energy entries written: {}", energySink.GetWrittenCount());
        logger_->info("Onset entries written: {}", onsetSink.GetWrittenCount());
        logger_->info("===========================\n");
    }
}
