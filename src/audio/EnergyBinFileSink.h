//
// Created by Robert on 2025-10-17.
//

#pragma once
#include "IAudioSource.h"
#include "core/CopySink.h"
#include <fstream>

namespace bpmfinder::audio
{
    class EnergyBinFileSink : public core::CopySink<float>
    {
    private:
        std::string filename_;
        std::ofstream file_; // RAII: The file handle is managed here!
        std::atomic<size_t> write_count_{0}; // Total items written to the file

    public:
        // 1. Constructor: Acquire Resource (Open File)
        explicit EnergyBinFileSink(const std::string& filename)
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

    protected:
        void Process(float data) override
        {
            std::cout << "Writing " << data << " energy value to file " << filename_ << "..." << std::endl;
            file_.write(reinterpret_cast<const char*>(&data), sizeof(float));
            ++write_count_;
            std::cout << "File size: " << file_.tellp() << " after " << GetWrittenCount() << " entries" << std::endl;
        }
    };
}
