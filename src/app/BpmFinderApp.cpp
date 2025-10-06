//
// Created by Robert on 2025-09-17.
//

#include "BpmFinderApp.h"
#include <chrono>
#include <iostream>
#include <thread>

#include "audio/AudioBinFileSink.h"
#include "audio/WasapiAudioSource.h"

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

        audio::WasapiAudioSource source(chunkSize);
        if (!source.Initialize())
        {
            std::cerr << "Failed to init WASAPI source" << std::endl;
            return;
        }

        audio::AudioBinFileSink sink("waveform.bin");

        source.Subscribe(&sink);

        // Start the sink's worker thread BEFORE starting the audio source
        sink.Start();

        source.Start();

        running_ = true;
        while (running_)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        source.Stop();
    }

    void BpmFinderApp::Stop()
    {
        running_ = false;
    }
}
