//
// Created by Robert on 2025-09-17.
//

#include "BpmFinderAppFactory.h"

namespace bpmfinder::app {

    std::unique_ptr<BpmFinderApp> BpmFinderAppFactory::CreateProductionApp() {
        return std::make_unique<BpmFinderApp>();
    }

    std::unique_ptr<BpmFinderApp> BpmFinderAppFactory::CreateTestApp() {
        return std::make_unique<BpmFinderApp>();
    }
}