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
    class DominantIntervalCalculationStage : public core::CopyStage<std::vector<float>, float>
    {
    public:
        explicit DominantIntervalCalculationStage()
            : logger_(logging::LoggerFactory::GetLogger("DominantIntervalCalculationStage"))
        {
            logger_->info("DominantIntervalCalculationStage initialized");
        }

    protected:
        void Process(std::vector<float> interOnsetIntervals) override
        {
            if (interOnsetIntervals.empty())
                return;

            // Method 1: Use median interval (simple and robust to outliers)
            std::vector<float> sortedIntervals = interOnsetIntervals;
            std::sort(sortedIntervals.begin(), sortedIntervals.end());

            const size_t medianIdx = sortedIntervals.size() / 2;
            float medianInterval = sortedIntervals[medianIdx];

            // Method 2 (Alternative): Use autocorrelation to find periodicity
            // This is more sophisticated but also more computationally expensive
            // For now, we use the simpler median approach


            this->Notify(medianInterval);
        }

    private:
        std::shared_ptr<spdlog::logger> logger_;
    };
}
