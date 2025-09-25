//
// Created by Robert on 2025-09-25.
//

#include "FileAudioSource.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace bpmfinder::audio;

FileAudioSource::FileAudioSource(const std::string& filename, const size_t chunkSize) :
    filename_(filename),
    chunkSize_(chunkSize)
{
}

FileAudioSource::~FileAudioSource()
{
    FileAudioSource::Stop();
}

bool FileAudioSource::Initialize()
{
    // A second call to Initialize should just rewind to the start of the file, no need to read in the whole file again
    if (samples_.size() > 0)
    {
        cursor_ = 0;
        return true;
    }

    // For now just read raw floats from a simple text file
    std::ifstream file(filename_);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename_ << std::endl;
        return false;
    }

    float value;
    while (file >> value)
    {
        samples_.push_back(value);
    }

    return !samples_.empty();
}

void FileAudioSource::Start()
{
    running_ = true;
    worker_ = std::thread(&FileAudioSource::CaptureLoop, this);
}

void FileAudioSource::CaptureLoop()
{
    while (running_ && samples_.size() > 0)
    {
        auto samplesToReturn = GetNextChunk(chunkSize_);
        Notify(samplesToReturn);
    }
}

std::vector<float> FileAudioSource::GetNextChunk(size_t n)
{
    const size_t end = std::min(cursor_ + n, samples_.size());
    std::vector<float> chunk(samples_.begin() + cursor_, samples_.begin() + end);
    cursor_ = end;
    return chunk;
}

void FileAudioSource::Stop()
{
    running_ = false;
    if (worker_.joinable())
    {
        worker_.join();
    }
}
