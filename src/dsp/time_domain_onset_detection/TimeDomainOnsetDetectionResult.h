//
// Created by Robert on 2025-10-19.
//

#pragma once
#include <vector>
#include <optional>
#include "audio/IAudioSource.h"

namespace bpmfinder::dsp::time_domain_onset_detection
{
    struct TimeDomainOnsetDetectionResult
    {
        // Identification
        size_t chunkIndex;

        // Processing parameters (set once, passed through)
        int sampleRate;
        int chunkSize;
        int bandPassLowCutoff;
        int bandPassHighCutoff;
        float bandPassGain;

        // Stage results (1:1 with chunk)
        audio::AudioChunk rawAudio;
        audio::AudioChunk bandPassFiltered;
        float energy = 0.0f;
        float onsetStrength = 0.0f;

        // Aggregated results (not 1:1, only available when accumulated)
        std::optional<std::vector<size_t>> peakIndices;
        std::optional<std::vector<float>> interOnsetIntervals;
        std::optional<float> dominantInterval;
        std::optional<float> bpm;

        TimeDomainOnsetDetectionResult(const size_t index, audio::AudioChunk chunk, const int sampleRate,
                                       const int chunkSize,
                                       const int bandPassLowCutoff,
                                       const int bandPassHighCutoff,
                                       const float bandPassGain)
            : chunkIndex(index), rawAudio(std::move(chunk)), sampleRate(sampleRate), chunkSize(chunkSize),
              bandPassLowCutoff(bandPassLowCutoff), bandPassHighCutoff(bandPassHighCutoff), bandPassGain(bandPassGain)
        {
        }
    };
}
