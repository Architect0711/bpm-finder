//
// Created by Robert on 2025-09-17.
//

#pragma once
#include <memory>
#include "BpmFinderApp.h"

namespace bpmfinder::app
{
    class BpmFinderAppFactory
    {
    public:
        static std::unique_ptr<BpmFinderApp> CreateProductionApp();

        static std::unique_ptr<BpmFinderApp> CreateTestApp();

    private:
        static void InitializeLogging(bool isProduction);
    };
}
