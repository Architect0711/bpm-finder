
//
// Created by Robert on 2025-10-19.
//

#pragma once
#include "core/CopyStage.h"
#include "logging/LoggerFactory.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace bpmfinder::dsp::time_domain_onset_detection
{
    class BpmCalculationStage : public core::CopyStage<float, float>
    {
    public:
        explicit BpmCalculationStage(const int sampleRate, const int chunkSize,
                                     int windowSizeSeconds = 15,
                                     float peakThreshold = 0.6f)
            : sampleRate_(sampleRate),
              chunkSize_(chunkSize),
              windowSizeSeconds_(windowSizeSeconds),
              peakThreshold_(peakThreshold),
              logger_(logging::LoggerFactory::GetLogger("BpmCalculationStage"))
        {
            // Calculate how many onset values we need to buffer
            // Each chunk produces one onset value
            // Chunks per second = sampleRate / chunkSize
            const float chunksPerSecond = static_cast<float>(sampleRate) / static_cast<float>(chunkSize);
            maxBufferSize_ = static_cast<size_t>(chunksPerSecond * windowSizeSeconds);

            logger_->info("BpmCalculationStage initialized - Window: {}s, Buffer size: {}, Peak threshold: {}",
                          windowSizeSeconds, maxBufferSize_, peakThreshold);
        }

        [[nodiscard]] float GetCurrentBpm() const { return currentBpm_; }

    protected:
        void Process(const float onsetStrength) override
        {
            // Add onset strength to buffer
            onsetBuffer_.push_back(onsetStrength);

            // Keep buffer size limited to our window
            if (onsetBuffer_.size() > maxBufferSize_)
            {
                onsetBuffer_.erase(onsetBuffer_.begin());
            }

            // Only calculate BPM once we have enough data
            if (onsetBuffer_.size() >= maxBufferSize_)
            {
                currentBpm_ = CalculateBpm();
                ++calculationCount_;

                if (calculationCount_ % 10 == 0) // Emit every 10 calculations
                {
                    logger_->warn("BPM: {:.1f}", currentBpm_);
                    this->Notify(currentBpm_);
                }
            }
        }

    private:
        float CalculateBpm()
        {
            // 1. Find peaks in the onset signal
            const std::vector<size_t> peakIndices = FindPeaks();

            if (peakIndices.size() < 2)
            {
                return currentBpm_; // Not enough peaks, return previous BPM
            }

            // 2. Calculate inter-onset intervals (IOI) in samples
            std::vector<float> intervals;
            intervals.reserve(peakIndices.size() - 1);

            for (size_t i = 1; i < peakIndices.size(); ++i)
            {
                float interval = static_cast<float>(peakIndices[i] - peakIndices[i - 1]);
                intervals.push_back(interval);
            }

            // 3. Use autocorrelation or median to find dominant interval
            const float dominantIntervalInSamples = CalculateDominantInterval(intervals);

            // 4. Convert interval from "onset buffer indices" to seconds
            // Each index in onsetBuffer represents one chunk
            const float chunksPerSecond = static_cast<float>(sampleRate_) / static_cast<float>(chunkSize_);

            // 5. Convert to BPM: BPM = 60 / period_in_seconds
            if (const float intervalInSeconds = dominantIntervalInSamples / chunksPerSecond; intervalInSeconds > 0.0f)
            {
                float bpm = 60.0f / intervalInSeconds;

                // Clamp to reasonable BPM range (30-240 BPM)
                bpm = std::clamp(bpm, 30.0f, 240.0f);

                return bpm;
            }

            return currentBpm_; // Keep previous BPM if calculation failed
        }

        std::vector<size_t> FindPeaks()
        {
            std::vector<size_t> peaks;

            // Calculate dynamic threshold based on buffer statistics
            const float maxValue = *std::max_element(onsetBuffer_.begin(), onsetBuffer_.end());
            const float threshold = maxValue * peakThreshold_;

            // Simple peak detection: local maximum that exceeds threshold
            for (size_t i = 1; i < onsetBuffer_.size() - 1; ++i)
            {
                if (onsetBuffer_[i] > threshold &&
                    onsetBuffer_[i] > onsetBuffer_[i - 1] &&
                    onsetBuffer_[i] > onsetBuffer_[i + 1])
                {
                    peaks.push_back(i);
                }
            }

            return peaks;
        }

        float CalculateDominantInterval(const std::vector<float>& intervals)
        {
            if (intervals.empty())
                return 0.0f;

            // Method 1: Use median interval (simple and robust to outliers)
            std::vector<float> sortedIntervals = intervals;
            std::sort(sortedIntervals.begin(), sortedIntervals.end());

            size_t medianIdx = sortedIntervals.size() / 2;
            float medianInterval = sortedIntervals[medianIdx];

            // Method 2 (Alternative): Use autocorrelation to find periodicity
            // This is more sophisticated but also more computationally expensive
            // For now, we use the simpler median approach

            return medianInterval;
        }

        int sampleRate_;
        int chunkSize_;
        int windowSizeSeconds_;
        float peakThreshold_;
        size_t maxBufferSize_;

        std::vector<float> onsetBuffer_;
        float currentBpm_ = 120.0f; // Default BPM
        size_t calculationCount_ = 0;

        std::shared_ptr<spdlog::logger> logger_;
    };
}
