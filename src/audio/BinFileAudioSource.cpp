//
// Created by Robert on 2025-09-25.
//

#include "BinFileAudioSource.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace bpmfinder::audio;

BinFileAudioSource::BinFileAudioSource(const std::string& filename, const size_t chunkSize) :
    filename_(filename),
    chunkSize_(chunkSize)
{
}

BinFileAudioSource::~BinFileAudioSource()
{
    BinFileAudioSource::Stop();
}

bool BinFileAudioSource::Initialize()
{
    // A second call to Initialize should just rewind to the start of the file, no need to read in the whole file again
    if (!samples_.empty())
    {
        cursor_ = 0;
        return true;
    }

    // Read raw floats from binary file
    std::ifstream file(filename_, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename_ << std::endl;
        return false;
    }

    // Get file size
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read all floats at once
    size_t numSamples = fileSize / sizeof(float);
    samples_.resize(numSamples);
    file.read(reinterpret_cast<char*>(samples_.data()), fileSize);

    return !samples_.empty();
}

void BinFileAudioSource::Start()
{
    running_ = true;
    worker_ = std::thread(&BinFileAudioSource::CaptureLoop, this);
}

void BinFileAudioSource::CaptureLoop()
{
    while (running_ && samples_.size() > 0)
    {
        auto samplesToReturn = GetNextChunk(chunkSize_);
        Notify(samplesToReturn);
    }
}

std::vector<float> BinFileAudioSource::GetNextChunk(size_t n)
{
    const size_t end = std::min(cursor_ + n, samples_.size());
    std::vector<float> chunk(samples_.begin() + cursor_, samples_.begin() + end);
    cursor_ = end;
    return chunk;
}

void BinFileAudioSource::Stop()
{
    running_ = false;
    if (worker_.joinable())
    {
        worker_.join();
    }
}
