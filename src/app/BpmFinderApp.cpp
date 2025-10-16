//
// Created by Robert on 2025-09-17.
//

#include "BpmFinderApp.h"
#include <chrono>
#include <thread>

#include "audio/AudioBinFileSink.h"
#include "dsp/time_domain_onset_detection/TimeDomainOnsetDetectionDspPipeline.h"

namespace bpmfinder::app
{
    BpmFinderApp::BpmFinderApp() : running_(false)
    {
    }

    BpmFinderApp::~BpmFinderApp() { Stop(); }

    void BpmFinderApp::Run()
    {
        auto chunkSize = 1024;
        auto sampleRate = 48000;
        auto bandPassLowCutoff = 100;
        auto bandPassHighCutoff = 10000;

        dsp::time_domain_onset_detection::TimeDomainOnsetDetectionDspPipeline
            dspPipeline(chunkSize, sampleRate, bandPassLowCutoff, bandPassHighCutoff);

        dspPipeline.Start();

        running_ = true;
        while (running_)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        dspPipeline.Stop();
    }

    void BpmFinderApp::Stop()
    {
        running_ = false;
    }
}
