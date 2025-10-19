//
// Created by Robert on 2025-09-17.
//

#include "BpmFinderAppFactory.h"
#include "logging/LoggerFactory.h"
#include <spdlog/spdlog.h>


namespace bpmfinder::app
{
    std::unique_ptr<BpmFinderApp> BpmFinderAppFactory::CreateProductionApp()
    {
        InitializeLogging(true);

        const auto logger = logging::LoggerFactory::GetLogger("BpmFinderAppFactory");
        logger->info("Creating production app");

        return std::make_unique<BpmFinderApp>();
    }

    std::unique_ptr<BpmFinderApp> BpmFinderAppFactory::CreateTestApp()
    {
        InitializeLogging(false);

        const auto logger = logging::LoggerFactory::GetLogger("BpmFinderAppFactory");
        logger->info("Creating test app");

        return std::make_unique<BpmFinderApp>();
    }

    void BpmFinderAppFactory::InitializeLogging(bool isProduction)
    {
        if (isProduction)
        {
            logging::LoggerFactory::Initialize(spdlog::level::debug);
        }
        else
        {
            logging::LoggerFactory::Initialize(spdlog::level::debug);
        }
    }
}
