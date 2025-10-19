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
    class InterOnsetIntervalCalculationStage : public core::CopyStage<
            TimeDomainOnsetDetectionResult, TimeDomainOnsetDetectionResult>
    {
    public:
        explicit InterOnsetIntervalCalculationStage()
            : logger_(logging::LoggerFactory::GetLogger("InterOnsetIntervalCalculationStage"))
        {
            logger_->info("InterOnsetIntervalCalculationStage initialized");
        }

    protected:
        void Process(TimeDomainOnsetDetectionResult data) override
        {
            // Need at least 2 peaks to calculate intervals
            if (data.peakIndices.has_value() && data.peakIndices.value().size() > 2)
            {
                // Calculate inter-onset intervals (IOI) in samples
                std::vector<float> intervals;
                intervals.reserve(data.peakIndices.value().size() - 1);

                for (size_t i = 1; i < data.peakIndices.value().size(); ++i)
                {
                    float interval = static_cast<float>(data.peakIndices.value()[i] - data.peakIndices.value()[i - 1]);
                    intervals.push_back(interval);
                }
            }

            this->Notify(data);
        }

    private:
        std::shared_ptr<spdlog::logger> logger_;
    };
}
