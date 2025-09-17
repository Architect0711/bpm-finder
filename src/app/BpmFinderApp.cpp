//
// Created by Robert on 2025-09-17.
//

#include "BpmFinderApp.h"
#include <chrono>
#include <iostream>
#include <thread>

namespace bpmfinder::app
{
    BpmFinderApp::BpmFinderApp() : running_(false)
    {
    }

    BpmFinderApp::~BpmFinderApp() { Stop(); }

    void BpmFinderApp::Run()
    {
        running_ = true;

        while (running_)
        {
            std::cout << "Hello, CLion!" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void BpmFinderApp::Stop()
    {
        running_ = false;
    }
}
