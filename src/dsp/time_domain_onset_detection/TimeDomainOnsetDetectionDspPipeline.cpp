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
        const int bandPassHighCutoff, const float bandPassGain)
        :
        chunkSize(chunkSize),
        sampleRate(sampleRate),
        bandPassLowCutoff(bandPassLowCutoff),
        bandPassHighCutoff(bandPassHighCutoff),
        bandPassGain(bandPassGain),
        source(chunkSize),
        initializationStage(sampleRate, chunkSize, bandPassLowCutoff, bandPassHighCutoff, bandPassGain),
        bandPassFilterStage(bandPassLowCutoff, bandPassHighCutoff, bandPassGain, sampleRate),
        sink("waveform.bin"),
        logger_(logging::LoggerFactory::GetLogger("TimeDomainOnsetDetectionDspPipeline"))
    {
        // In the ctor we only assemble the dsp chain, start reading audio data and processing it via Start()
        if (!source.Initialize())
        {
            logger_->error("Failed to init WASAPI source");
            return;
        }

        source.Subscribe(&sink); // Write raw input data to file
        source.Subscribe(&initializationStage); // Pass raw input data to initialization stage

        initializationStage.Subscribe(&bandPassFilterStage); // Pass raw input data to bandpass filter stage

        bandPassFilterStage.Subscribe(&energyCalculationStage); // Pass bandpass filtered data to energy calc stage

        energyCalculationStage.Subscribe(&onsetDetectionStage); // Pass energy data to onset detection stage

        onsetDetectionStage.Subscribe(&peakIndexDetectionStage); // Pass onset data to peak index detection stage

        peakIndexDetectionStage.Subscribe(&interOnsetIntervalCalculationStage);
        // Pass peak index data to inter onset interval calc stage

        interOnsetIntervalCalculationStage.Subscribe(&dominantIntervalCalculationStage);
        // Pass inter onset interval data to dominant interval calc stage

        dominantIntervalCalculationStage.Subscribe(&bpmCalculationStage);
        // Pass dominant interval data to bpm calc stage
    }

    void TimeDomainOnsetDetectionDspPipeline::Start()
    {
        // Start the pipeline's worker threads BEFORE starting the audio source
        sink.Start();
        initializationStage.Start();
        bandPassFilterStage.Start();
        energyCalculationStage.Start();
        onsetDetectionStage.Start();
        peakIndexDetectionStage.Start();
        interOnsetIntervalCalculationStage.Start();
        dominantIntervalCalculationStage.Start();
        bpmCalculationStage.Start();

        source.Start();
    }

    void TimeDomainOnsetDetectionDspPipeline::Stop()
    {
        // First stop the data flow from the audio source and then the rest of the pipeline
        source.Stop();

        // Then stop the pipeline stages in REVERSE order to allow them to drain their queues
        // Stop sinks last to ensure all processed data is written
        initializationStage.StopAndDrain();
        bandPassFilterStage.StopAndDrain();
        energyCalculationStage.StopAndDrain();
        onsetDetectionStage.StopAndDrain();
        peakIndexDetectionStage.StopAndDrain();
        interOnsetIntervalCalculationStage.StopAndDrain();
        dominantIntervalCalculationStage.StopAndDrain();
        bpmCalculationStage.StopAndDrain();

        sink.StopAndDrain();

        // Print statistics
        logger_->info("\n");
        logger_->info("=== Pipeline Statistics ===");
        logger_->info("Sink: {}/{}", sink.GetProcessedCount(), sink.GetQueuedCount());
        logger_->info("BandPassFilterStage: {}/{}",
                      bandPassFilterStage.GetProcessedCount(), bandPassFilterStage.GetQueuedCount());
        logger_->info("EnergyCalculationStage: {}/{}",
                      energyCalculationStage.GetProcessedCount(), energyCalculationStage.GetQueuedCount());
        logger_->info("OnsetDetectionStage: {}/{}",
                      onsetDetectionStage.GetProcessedCount(), onsetDetectionStage.GetQueuedCount());
        logger_->info("PeakIndexDetectionStage: {}/{}", peakIndexDetectionStage.GetProcessedCount(),
                      peakIndexDetectionStage.GetQueuedCount());
        logger_->info("InterOnsetIntervalCalculationStage: {}/{}",
                      interOnsetIntervalCalculationStage.GetProcessedCount(),
                      interOnsetIntervalCalculationStage.GetQueuedCount());
        logger_->info("DominantIntervalCalculationStage: {}/{}", dominantIntervalCalculationStage.GetProcessedCount(),
                      dominantIntervalCalculationStage.GetQueuedCount());
        logger_->info("BpmCalculationStage: {}/{}", bpmCalculationStage.GetProcessedCount(),
                      bpmCalculationStage.GetQueuedCount());
        logger_->info("Waveform entries written: {}", sink.GetWrittenCount());
        logger_->info("===========================\n");
    }
}
