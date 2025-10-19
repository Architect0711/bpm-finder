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
    class PeakIndexDetectionStage : public core::CopyStage<
            TimeDomainOnsetDetectionResult, TimeDomainOnsetDetectionResult>
    {
    public:
        explicit PeakIndexDetectionStage(int slidingWindowSizeSeconds = 15,
                                         float peakThreshold = 0.6f)
            : slidingWindowSizeSeconds_(slidingWindowSizeSeconds),
              peakThreshold_(peakThreshold),
              logger_(logging::LoggerFactory::GetLogger("PeakIndexDetectionStage"))
        {
            logger_->info(
                "PeakIndexDetectionStage initialized - Sliding Window: {}s, Buffer size: {}, Peak threshold: {}",
                slidingWindowSizeSeconds, maxBufferSize_, peakThreshold);
        }

    protected:
        void Process(TimeDomainOnsetDetectionResult data) override
        {
            // Calculate how many onset values we need to buffer
            // Each chunk produces one onset value
            // Chunks per second = sampleRate / chunkSize
            const float chunksPerSecond = static_cast<float>(data.sampleRate) / static_cast<float>(data.chunkSize);
            maxBufferSize_ = static_cast<size_t>(chunksPerSecond * slidingWindowSizeSeconds_);

            // Add onset strength to buffer
            onsetBuffer_.push_back(data.onsetStrength);

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

                // Further processing only makes sense for 2 or more peaks
                if (peaks.size() >= 2)
                {
                    data.peakIndices = peaks;
                }
            }

            this->Notify(data);
        }

    private:
        int slidingWindowSizeSeconds_;
        float peakThreshold_;
        size_t maxBufferSize_;
        std::vector<float> onsetBuffer_;

        std::shared_ptr<spdlog::logger> logger_;
    };
}
