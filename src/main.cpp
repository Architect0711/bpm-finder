//
// Created by Robert on 2025-09-08.
//

#include <iostream>

#include "app/BpmFinderApp.h"
#include "app/BpmFinderAppFactory.h"

using namespace bpmfinder::app;

int main() {
    const auto app = BpmFinderAppFactory::CreateProductionApp();
    app->Run(); // blocks until stopped
    return 0;
}
