//
// Created by Robert on 2025-09-08.
//

#include <iostream>
#include <csignal>
#include <memory>

#include "app/BpmFinderApp.h"
#include "app/BpmFinderAppFactory.h"

using namespace bpmfinder::app;

// Global pointer to app for signal handler
std::unique_ptr<BpmFinderApp> g_app;

void signalHandler(int signum)
{
    std::cout << "\n=== Interrupt signal (" << signum << ") received ===" << std::endl;
    if (g_app)
    {
        g_app->Stop();
    }
}

int main()
{
    // Register signal handlers for graceful shutdown
    std::signal(SIGINT, signalHandler); // Ctrl+C
    std::signal(SIGTERM, signalHandler); // CLion stop button

    g_app = BpmFinderAppFactory::CreateProductionApp();
    g_app->Run(); // blocks until stopped

    std::cout << "Application stopped gracefully." << std::endl;
    return 0;
}
