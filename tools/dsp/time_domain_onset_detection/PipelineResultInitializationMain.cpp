//
// Created by Robert on 2025-10-20.
//

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <csignal>
#include <vector>

#include <nlohmann/json.hpp>

#include "JsonTools.h"
#include "StageTestObservable.h"
#include "StageTestObserver.h"
#include "dsp/time_domain_onset_detection/PipelineResultInitializationStage.h"
#include "dsp/time_domain_onset_detection/TimeDomainOnsetDetectionResult.h"

using json = nlohmann::json;

std::atomic<bool> running = true;

void signalHandler(const int signum)
{
    std::cout << "\n=== Interrupt signal (" << signum << ") received ===" << std::endl;
    running = false;
}

int main(const int argc, char* argv[])
{
    if (argc < 6)
    {
        std::cerr <<
            "[ERR] Usage: {path_to_executable} <sampleRate (int)> <chunkSize (int)> <bandPassLowCutoff (int)> <bandPassHighCutoff (int)> <bandPassGain (float)>"
            << std::endl;
        return 1;
    }

    // Register signal handlers for graceful shutdown
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    int sampleRate = std::atoi(argv[1]);
    int chunkSize = std::atoi(argv[2]);
    int bandPassLowCutoff = std::atoi(argv[3]);
    int bandPassHighCutoff = std::atoi(argv[4]);
    float bandPassGain = std::atof(argv[5]);

    // Create the stage we want to test in isolation
    bpmfinder::dsp::time_domain_onset_detection::PipelineResultInitializationStage stage(
        sampleRate, chunkSize, bandPassLowCutoff, bandPassHighCutoff, bandPassGain);

    // Create an observer that will capture the output of the stage
    auto observer = bpmfinder::tools::dsp::time_domain_onset_detection::StageTestObserver<
        bpmfinder::dsp::time_domain_onset_detection::TimeDomainOnsetDetectionResult>();
    stage.Subscribe(&observer);

    // Create an observable that will publish the input of the stage
    auto source = bpmfinder::tools::dsp::time_domain_onset_detection::StageTestObservable<
        bpmfinder::audio::AudioChunk>();
    source.Subscribe(&stage);

    std::string line;
    while (running && std::getline(std::cin, line))
    {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);

        if (line.empty())
        {
            continue;
        }

        // Assume it's JSON input with audio samples
        try
        {
            auto audioChunk = bpmfinder::tools::dsp::time_domain_onset_detection::ParseJsonAudioChunk(line);
            source.Publish(audioChunk);
        }
        catch (const std::exception& e)
        {
            std::cerr << std::string("[ERR] Failed to parse JSON: ") + e.what() << std::endl;
        }

        std::cout.flush();
    }

    std::cout << "[LOG] shutdown" << std::endl;

    return 0;
}
