//
// Created by Robert on 2025-10-22.
//

#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "dsp/time_domain_onset_detection/TimeDomainOnsetDetectionResult.h"

using json = nlohmann::json;


namespace bpmfinder::tools::dsp::time_domain_onset_detection
{
    // Parse JSON input to extract audio chunk
    inline std::vector<float> ParseJsonAudioChunk(const std::string& jsonStr)
    {
        try
        {
            json j = json::parse(jsonStr);

            if (!j.contains("rawAudio") || !j["rawAudio"].is_array())
            {
                throw std::runtime_error("JSON must contain 'rawAudio' array");
            }

            return j["rawAudio"].get<std::vector<float>>();
        }
        catch (const json::parse_error& e)
        {
            throw std::runtime_error(std::string("JSON parse error: ") + e.what());
        }
    }

    // Serialize result to JSON
    inline nlohmann::json SerializeResultToJson(
        const bpmfinder::dsp::time_domain_onset_detection::TimeDomainOnsetDetectionResult& result)
    {
        json j;
        j["chunkIndex"] = result.chunkIndex;
        j["sampleRate"] = result.sampleRate;
        j["chunkSize"] = result.chunkSize;
        j["bandPassLowCutoff"] = result.bandPassLowCutoff;
        j["bandPassHighCutoff"] = result.bandPassHighCutoff;
        j["bandPassGain"] = result.bandPassGain;
        j["rawAudio"] = result.rawAudio;

        return j;
    }
}

