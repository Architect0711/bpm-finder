//
// Created by Robert on 2025-10-17.
//

#pragma once
#include <atomic>
#include <iosfwd>
#include <fstream>
#include <stdexcept>
#include <string>
#include "core/CopySink.h"
#include <spdlog/spdlog.h>


namespace bpmfinder::files::bin
{
    template <typename T>
    class BinFileSink : public core::CopySink<T>
    {
    protected:
        std::string filename_;
        std::ofstream file_; // RAII: The file handle is managed here!
        std::atomic<size_t> write_count_{0}; // Total items written to the file
        std::shared_ptr<spdlog::logger> logger_;

    public:
        // 1. Constructor: Acquire Resource (Open File)
        explicit BinFileSink(const std::string& filename, std::shared_ptr<spdlog::logger> logger) : filename_(filename),
            logger_(std::move(logger))
        {
            // Opening the file here is the "Initialization"
            file_.open(filename, std::ios::binary | std::ios::trunc);
            if (!file_.is_open())
            {
                throw std::runtime_error("Failed to open file: " + filename);
            }
        }

        // 2. Destructor: Resource is Automatically Released
        // When FileSink is destroyed, its member file_ is destroyed,
        // and std::ofstream's destructor automatically calls file_.close().
        // You do NOT need a separate Shutdown method.

        [[nodiscard]] size_t GetWrittenCount() const { return write_count_.load(); }
    };
}
