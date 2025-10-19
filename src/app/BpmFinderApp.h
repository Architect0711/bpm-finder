//
// Created by Robert on 2025-09-17.
//

#pragma once
#include <atomic>
#include "spdlog/logger.h"

namespace bpmfinder::app
{
    class BpmFinderApp
    {
    public:
        BpmFinderApp();
        ~BpmFinderApp();

        // Start the main daemon loop (blocking)
        void Run();

        // Stop the loop gracefully
        void Stop();

    private:
        std::atomic<bool> running_;
        std::shared_ptr<spdlog::logger> logger_;
    };
}
