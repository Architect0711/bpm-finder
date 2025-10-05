//
// Created by Robert on 2025-09-25.
//

#pragma once
#include "IAudioSource.h"
#include "core/Sink.h"
#include <fstream>

namespace bpmfinder::audio
{
    class AudioBinFileSink : public core::Sink<AudioChunk>
    {
    private:
        std::ofstream file_; // RAII: The file handle is managed here!

    public:
        // 1. Constructor: Acquire Resource (Open File)
        explicit AudioBinFileSink(const std::string& filename)
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

    protected:
        void Process(AudioChunk data) override
        {
            // Since AudioChunk is std::vector<float>:
            file_.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(float));
        }
    };
}

