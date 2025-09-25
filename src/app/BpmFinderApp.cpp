//
// Created by Robert on 2025-09-17.
//

#include "BpmFinderApp.h"
#include <chrono>
#include <iostream>
#include <thread>

#include "audio/AudioCsvFileSink.h"
#include "audio/WasapiAudioSource.h"

namespace bpmfinder::app
{
    BpmFinderApp::BpmFinderApp() : running_(false)
    {
    }

    BpmFinderApp::~BpmFinderApp() { Stop(); }

    void BpmFinderApp::Run()
    {
        audio::WasapiAudioSource source;
        if (!source.Initialize())
        {
            std::cerr << "Failed to init WASAPI source" << std::endl;
            return;
        }

        audio::AudioCsvFileSink sink("waveform.csv");

        source.Subscribe(&sink);

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
