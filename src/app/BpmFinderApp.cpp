//
// Created by Robert on 2025-09-17.
//

#include "BpmFinderApp.h"
#include <chrono>
#include <thread>

#include "../files/bin/AudioBinFileSink.h"
#include "dsp/time_domain_onset_detection/TimeDomainOnsetDetectionDspPipeline.h"

namespace bpmfinder::app
{
    BpmFinderApp::BpmFinderApp() : running_(false), logger_(logging::LoggerFactory::GetLogger("BpmFinderApp"))
    {
    }

    BpmFinderApp::~BpmFinderApp() { Stop(); }

    void BpmFinderApp::Run()
    {
        auto chunkSize = 1024;
        auto sampleRate = 48000;
        auto bandPassLowCutoff = 40;
        auto bandPassHighCutoff = 800;
        auto bandPassGain = 1.0f;

        dsp::time_domain_onset_detection::TimeDomainOnsetDetectionDspPipeline
            dspPipeline(chunkSize, sampleRate, bandPassLowCutoff, bandPassHighCutoff, bandPassGain);

        dspPipeline.Start();

        running_ = true;
        /**
        while (running_)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
         **/

        // Record for 10 seconds then stop gracefully
        constexpr int recordingDurationSeconds = 30;
        logger_->info("Recording for {} seconds...", recordingDurationSeconds);
        for (int i = 0; i < recordingDurationSeconds && running_; ++i)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            logger_->info("Recording... {}/{} seconds", i + 1, recordingDurationSeconds);
        }

        dspPipeline.Stop();
    }

    void BpmFinderApp::Stop()
    {
        running_ = false;
    }
}
