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
    class InterOnsetIntervalCalculationStage : public core::CopyStage<std::vector<size_t>, std::vector<float>>
    {
    public:
        explicit InterOnsetIntervalCalculationStage()
            : logger_(logging::LoggerFactory::GetLogger("InterOnsetIntervalCalculationStage"))
        {
            logger_->info("InterOnsetIntervalCalculationStage initialized");
        }

    protected:
        void Process(const std::vector<size_t> peakIndices) override
        {
            // Need at least 2 peaks to calculate intervals
            if (peakIndices.size() < 2)
            {
                return; // Don't notify downstream if we can't calculate intervals
            }

            // Calculate inter-onset intervals (IOI) in samples
            std::vector<float> intervals;
            intervals.reserve(peakIndices.size() - 1);

            for (size_t i = 1; i < peakIndices.size(); ++i)
            {
                float interval = static_cast<float>(peakIndices[i] - peakIndices[i - 1]);
                intervals.push_back(interval);
            }

            this->Notify(intervals);
        }

    private:
        std::shared_ptr<spdlog::logger> logger_;
    };
}
