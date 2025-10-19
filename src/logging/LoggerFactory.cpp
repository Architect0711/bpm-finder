//
// Created by Robert on 2025-10-18.
//

#include "LoggerFactory.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace bpmfinder::logging
{
    bool LoggerFactory::initialized_ = false;
    std::vector<spdlog::sink_ptr> LoggerFactory::sinks_;

    void LoggerFactory::Initialize(spdlog::level::level_enum defaultLevel)
    {
        if (initialized_)
        {
            spdlog::warn("LoggerFactory already initialized");
            return;
        }

        // Create console sink with color support
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(defaultLevel);
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");

        sinks_.push_back(console_sink);

        // Set the default logger
        auto default_logger = std::make_shared<spdlog::logger>("default", sinks_.begin(), sinks_.end());
        default_logger->set_level(defaultLevel);
        spdlog::set_default_logger(default_logger);

        initialized_ = true;
        spdlog::info("Logging system initialized");
    }

    void LoggerFactory::Initialize(const std::string& logFilePath,
                                   const spdlog::level::level_enum consoleLevel,
                                   const spdlog::level::level_enum fileLevel)
    {
        if (initialized_)
        {
            spdlog::warn("LoggerFactory already initialized");
            return;
        }

        // Create console sink
        const auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(consoleLevel);
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");

        // Create file sink
        const auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath, true);
        file_sink->set_level(fileLevel);
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [thread %t] %v");

        sinks_.push_back(console_sink);
        sinks_.push_back(file_sink);

        // Set the default logger
        const auto default_logger = std::make_shared<spdlog::logger>("default", sinks_.begin(), sinks_.end());
        default_logger->set_level(std::min(consoleLevel, fileLevel));
        spdlog::set_default_logger(default_logger);

        initialized_ = true;
        spdlog::info("Logging system initialized with file: {}", logFilePath);
    }

    std::shared_ptr<spdlog::logger> LoggerFactory::GetLogger(const std::string& name)
    {
        if (!initialized_)
        {
            Initialize(); // Auto-initialize with defaults if not done
        }

        // Try to get existing logger
        auto logger = spdlog::get(name);
        if (logger)
        {
            return logger;
        }

        // Create new logger with shared sinks
        logger = std::make_shared<spdlog::logger>(name, sinks_.begin(), sinks_.end());
        logger->set_level(spdlog::level::trace); // Individual loggers log everything; sinks filter
        spdlog::register_logger(logger);

        return logger;
    }

    void LoggerFactory::SetLogLevel(const std::string& name, spdlog::level::level_enum level)
    {
        if (const auto logger = spdlog::get(name))
        {
            logger->set_level(level);
        }
    }

    void LoggerFactory::SetGlobalLogLevel(spdlog::level::level_enum level)
    {
        spdlog::set_level(level);
        for (const auto& sink : sinks_)
        {
            sink->set_level(level);
        }
    }

    void LoggerFactory::Shutdown()
    {
        spdlog::info("Shutting down logging system");
        spdlog::shutdown();
        sinks_.clear();
        initialized_ = false;
    }
}
