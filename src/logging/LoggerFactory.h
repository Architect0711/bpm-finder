//
// Created by Robert on 2025-10-18.
//

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>
#include <string>

namespace bpmfinder::logging
{
    class LoggerFactory
    {
    public:
        // Initialize the logging system with default configuration
        static void Initialize(spdlog::level::level_enum defaultLevel = spdlog::level::info);

        // Initialize with custom configuration
        static void Initialize(const std::string& logFilePath,
                               spdlog::level::level_enum consoleLevel,
                               spdlog::level::level_enum fileLevel);

        // Get or create a named logger
        static std::shared_ptr<spdlog::logger> GetLogger(const std::string& name);

        // Set log level for a specific logger
        static void SetLogLevel(const std::string& name, spdlog::level::level_enum level);

        // Set log level for all loggers
        static void SetGlobalLogLevel(spdlog::level::level_enum level);

        // Shutdown logging system (flush all loggers)
        static void Shutdown();

    private:
        static bool initialized_;
        static std::vector<spdlog::sink_ptr> sinks_;

        LoggerFactory() = default; // Prevent instantiation
    };
}
