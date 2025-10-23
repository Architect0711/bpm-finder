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
    // Disable output buffering for immediate visibility in pipes
    std::cout.setf(std::ios::unitbuf);

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

    std::cout << "[LOG] Sample Rate: " << sampleRate << " Chunk Size: " << chunkSize << " Bandpass: " <<
        bandPassLowCutoff << " - " << bandPassHighCutoff << " Gain: " << bandPassGain << std::endl;

    // Create the stage we want to test in isolation
    std::cout << "[LOG] Creating PipelineResultInitializationStage" << std::endl;
    bpmfinder::dsp::time_domain_onset_detection::PipelineResultInitializationStage stage(
        sampleRate, chunkSize, bandPassLowCutoff, bandPassHighCutoff, bandPassGain);

    // Create an observer that will capture the output of the stage
    std::cout << "[LOG] Creating StageTestObserver" << std::endl;
    auto observer = bpmfinder::tools::dsp::time_domain_onset_detection::StageTestObserver<
        bpmfinder::dsp::time_domain_onset_detection::TimeDomainOnsetDetectionResult>();
    stage.Subscribe(&observer);
    stage.Start();

    // Create an observable that will publish the input of the stage
    std::cout << "[LOG] Creating StageTestObservable" << std::endl;
    auto source = bpmfinder::tools::dsp::time_domain_onset_detection::StageTestObservable<
        bpmfinder::audio::AudioChunk>();
    source.Subscribe(&stage);

    std::cout << "[LOG] ready" << std::endl;

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

        std::cout << "[LOG] input: " << line << std::endl;

        if (line == "exit")
        {
            running = false;
            break;
        }

        // Assume it's JSON input with audio samples
        try
        {
            std::cout << "[LOG] Decoding Input Data..." << std::endl;
            auto audioChunk = bpmfinder::tools::dsp::time_domain_onset_detection::ParseJsonAudioChunk(line);
            std::cout << "[LOG] Decoded Input Data: " << audioChunk.size() << " Samples" << std::endl;
            source.Publish(audioChunk);
            std::cout << "[LOG] Called Stage..." << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << std::string("[ERR] Failed to parse JSON: ") + e.what() << std::endl;
        }

        // Wait for the observer to receive the result => it's asynchronous in another thread!
        auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(5);
        while (!observer.HasResult() && std::chrono::steady_clock::now() < timeout)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (observer.HasResult())
        {
            std::cout << "[LOG] Observer has " << observer.GetQueuedCount() << " results" << std::endl;
            auto results = observer.GetData();
            for (const auto& result : results)
            {
                std::cout << bpmfinder::tools::dsp::time_domain_onset_detection::SerializeResultToJson(result) <<
                    std::endl;
            }
        }
        else
        {
            std::cout << "[LOG] No results yet" << std::endl;
        }

        std::cout.flush();
    }

    std::cout << "[LOG] exit" << std::endl;

    return 0;
}
