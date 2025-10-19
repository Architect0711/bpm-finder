//
// Created by Robert on 2025-10-19.
//

#pragma once
#include <vector>
#include "core/CopyStage.h"
#include "logging/LoggerFactory.h"
#include "spdlog/logger.h"

namespace bpmfinder::dsp::time_domain_onset_detection
{
    class PeakIndexDetectionStage : public core::CopyStage<float, std::vector<size_t>>
    {
    public:
        explicit PeakIndexDetectionStage(const int sampleRate, const int chunkSize,
                                         int slidingWindowSizeSeconds = 15,
                                         float peakThreshold = 0.6f)
            : sampleRate_(sampleRate),
              chunkSize_(chunkSize),
              slidingWindowSizeSeconds_(slidingWindowSizeSeconds),
              peakThreshold_(peakThreshold),
              logger_(logging::LoggerFactory::GetLogger("PeakIndexDetectionStage"))
        {
            // Calculate how many onset values we need to buffer
            // Each chunk produces one onset value
            // Chunks per second = sampleRate / chunkSize
            const float chunksPerSecond = static_cast<float>(sampleRate) / static_cast<float>(chunkSize);
            maxBufferSize_ = static_cast<size_t>(chunksPerSecond * slidingWindowSizeSeconds);

            logger_->info(
                "PeakIndexDetectionStage initialized - Sliding Window: {}s, Buffer size: {}, Peak threshold: {}",
                slidingWindowSizeSeconds, maxBufferSize_, peakThreshold);
        }

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

            // Only calculate Peaks once we have enough data
            if (onsetBuffer_.size() >= maxBufferSize_)
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

                if (peaks.size() >= 2) // Only notify if we have at least 2 peaks
                {
                    this->Notify(peaks);
                }
            }
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

    private:
        int sampleRate_;
        int chunkSize_;
        int slidingWindowSizeSeconds_;
        float peakThreshold_;
        size_t maxBufferSize_;
        std::vector<float> onsetBuffer_;

        std::shared_ptr<spdlog::logger> logger_;
    };
}
