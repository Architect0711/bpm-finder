//
// Created by Robert on 2025-10-23.
//

#pragma once
#include <map>

#include "core/CopyStage.h"
#include "dsp/time_domain_onset_detection/BandPassFilterStage.h"
#include "dsp/time_domain_onset_detection/BpmCalculationStage.h"
#include "dsp/time_domain_onset_detection/DominantIntervalCalculationStage.h"
#include "dsp/time_domain_onset_detection/EnergyCalculationStage.h"
#include "dsp/time_domain_onset_detection/InterOnsetIntervalCalculationStage.h"
#include "dsp/time_domain_onset_detection/OnsetDetectionStage.h"
#include "dsp/time_domain_onset_detection/PeakIndexDetectionStage.h"
#include "dsp/time_domain_onset_detection/TimeDomainOnsetDetectionResult.h"

using StageType = bpmfinder::core::CopyStage<
    bpmfinder::dsp::time_domain_onset_detection::TimeDomainOnsetDetectionResult,
    bpmfinder::dsp::time_domain_onset_detection::TimeDomainOnsetDetectionResult>;

namespace bpmfinder::tools::dsp::time_domain_onset_detection
{
    class PipelineStageFactory
    {
    public:
        std::unique_ptr<StageType> CreatePipelineStage(const int argc, char* argv[])
        {
            if (argc < 2)
            {
                std::cerr << "[ERR] Usage: <stageName (string)> <args>" << std::endl;
                return nullptr;
            }

            const std::string stageName = argv[1];

            // Factory map for creating stages
            std::map<std::string, std::function<std::unique_ptr<StageType>()>> stageFactory;

            stageFactory["BandPassFilterStage"] = [&]() -> std::unique_ptr<StageType>
            {
                if (argc < 5)
                {
                    std::cerr <<
                        "[ERR] Usage: <stageName (string)>  <sampleRate (int)> <bandPassLowCutoff (int)> <bandPassHighCutoff (int)> <bandPassGain (float)>"
                        << std::endl;
                    return nullptr;
                }

                const int sampleRate = std::atoi(argv[2]);
                const int bandPassLowCutoff = std::atoi(argv[3]);
                const int bandPassHighCutoff = std::atoi(argv[4]);
                const float bandPassGain = std::atof(argv[5]);

                return std::make_unique<bpmfinder::dsp::time_domain_onset_detection::BandPassFilterStage>(
                    bandPassLowCutoff, bandPassHighCutoff, bandPassGain, sampleRate);
            };

            stageFactory["EnergyCalculationStage"] = [&]() -> std::unique_ptr<StageType>
            {
                return std::make_unique<bpmfinder::dsp::time_domain_onset_detection::EnergyCalculationStage>();
            };

            stageFactory["OnsetDetectionStage"] = [&]() -> std::unique_ptr<StageType>
            {
                return std::make_unique<bpmfinder::dsp::time_domain_onset_detection::OnsetDetectionStage>();
            };

            stageFactory["PeakIndexDetectionStage"] = [&]() -> std::unique_ptr<StageType>
            {
                return std::make_unique<bpmfinder::dsp::time_domain_onset_detection::PeakIndexDetectionStage>();
            };
            stageFactory["InterOnsetIntervalCalculationStage"] = [&]() -> std::unique_ptr<StageType>
            {
                return std::make_unique<
                    bpmfinder::dsp::time_domain_onset_detection::InterOnsetIntervalCalculationStage>();
            };

            stageFactory["DominantIntervalCalculationStage"] = [&]() -> std::unique_ptr<StageType>
            {
                return std::make_unique<
                    bpmfinder::dsp::time_domain_onset_detection::DominantIntervalCalculationStage>();
            };

            stageFactory["BpmCalculationStage"] = [&]() -> std::unique_ptr<StageType>
            {
                return std::make_unique<bpmfinder::dsp::time_domain_onset_detection::BpmCalculationStage>();
            };

            // Create the stage we want to test in isolation
            if (stageFactory.find(stageName) == stageFactory.end())
            {
                std::cerr << "[ERR] Unknown stage name: " << stageName << std::endl;
                std::cerr <<
                    "[ERR] Available stages: BandPassFilterStage, EnergyCalculationStage, OnsetDetectionStage, PeakIndexDetectionStage, InterOnsetIntervalCalculationStage, DominantIntervalCalculationStage, BpmCalculationStage"
                    << std::endl;
                return nullptr;
            }

            return stageFactory[stageName]();
        }
    };
};
