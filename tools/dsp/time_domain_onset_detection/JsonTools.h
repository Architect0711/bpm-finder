//
// Created by Robert on 2025-10-22.
//

#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "dsp/time_domain_onset_detection/TimeDomainOnsetDetectionResult.h"

using json = nlohmann::json;

using ResultObject = bpmfinder::dsp::time_domain_onset_detection::TimeDomainOnsetDetectionResult;


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
                std::cerr << "[ERR] JSON must contain 'rawAudio' array";
                return {};
            }

            return j["rawAudio"].get<std::vector<float>>();
        }
        catch (const json::parse_error& e)
        {
            std::cerr << std::string("[ERR] JSON parse error: ") + e.what();
        }

        return {};
    }

    inline std::unique_ptr<ResultObject> ParseJsonResult(
        const std::string& jsonStr)
    {
        try
        {
            json j = json::parse(jsonStr);

            // Required fields with defaults
            size_t chunkIndex = j.value("chunkIndex", 0);
            int sampleRate = j.value("sampleRate", 0);
            int chunkSize = j.value("chunkSize", 0);
            int bandPassLowCutoff = j.value("bandPassLowCutoff", 0);
            int bandPassHighCutoff = j.value("bandPassHighCutoff", 0);
            float bandPassGain = j.value("bandPassGain", 0.0f);

            // Parse rawAudio if present
            std::vector<float> rawAudio;
            if (j.contains("rawAudio") && j["rawAudio"].is_array())
            {
                rawAudio = j["rawAudio"].get<std::vector<float>>();
            }

            // Create result object with required fields
            ResultObject result(
                chunkIndex, rawAudio, sampleRate, chunkSize,
                bandPassLowCutoff, bandPassHighCutoff, bandPassGain);

            // Parse optional fields that may be populated by pipeline stages
            if (j.contains("bandPassFiltered") && j["bandPassFiltered"].is_array())
            {
                result.bandPassFiltered = j["bandPassFiltered"].get<std::vector<float>>();
            }

            result.energy = j.value("energy", 0.0f);
            result.onsetStrength = j.value("onsetStrength", 0.0f);

            // Parse optional aggregated results
            if (j.contains("peakIndices") && j["peakIndices"].is_array())
            {
                result.peakIndices = j["peakIndices"].get<std::vector<size_t>>();
            }

            if (j.contains("interOnsetIntervals") && j["interOnsetIntervals"].is_array())
            {
                result.interOnsetIntervals = j["interOnsetIntervals"].get<std::vector<float>>();
            }

            if (j.contains("dominantInterval") && !j["dominantInterval"].is_null())
            {
                result.dominantInterval = j["dominantInterval"].get<float>();
            }

            if (j.contains("bpm") && !j["bpm"].is_null())
            {
                result.bpm = j["bpm"].get<float>();
            }

            return std::make_unique<ResultObject>(result);
        }
        catch (const json::parse_error& e)
        {
            std::cerr << std::string("[ERR] JSON parse error: ") + e.what() << std::endl;

            return nullptr;
        }
    }

    // Serialize result to JSON
    inline nlohmann::json SerializeResultToJson(const ResultObject& result)
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

