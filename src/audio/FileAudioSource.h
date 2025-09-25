//
// Created by Robert on 2025-09-25.
//

#pragma once
#include "IAudioSource.h"
#include "core/Observable.h"
#include <vector>
#include <string>
#include <fstream>
#include <thread>

namespace bpmfinder::audio
{
    class FileAudioSource final : public IAudioSource
    {
    public:
        explicit FileAudioSource(const std::string& filename, size_t chunkSize = 512);
        ~FileAudioSource() override;

        bool Initialize() override;
        void Start() override;
        void Stop() override;

    private:
        void CaptureLoop();
        std::vector<float> GetNextChunk(size_t n);

        std::string filename_;
        size_t chunkSize_;
        size_t cursor_ = 0;
        std::vector<float> samples_;
        std::thread worker_;
        bool running_ = false;
    };
}
