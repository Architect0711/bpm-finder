//
// Created by Robert on 2025-09-24.
//

#include "WasapiAudioSource.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <combaseapi.h>

using namespace bpmfinder::audio;

WasapiAudioSource::WasapiAudioSource(size_t chunkSize)
    : waveFormat_(nullptr), eventHandle_(nullptr), capturing_(false), chunkSize_(chunkSize)
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    audioBuffer_.reserve(chunkSize_);
}

WasapiAudioSource::~WasapiAudioSource()
{
    Stop();
    if (eventHandle_) CloseHandle(eventHandle_);
    CoUninitialize();
}

bool WasapiAudioSource::Initialize()
{
    // Get default audio device
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                  IID_PPV_ARGS(&deviceEnumerator_));
    if (FAILED(hr)) return false;

    hr = deviceEnumerator_->GetDefaultAudioEndpoint(eRender, eConsole, &device_);
    if (FAILED(hr)) return false;

    hr = device_->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, &audioClient_);
    if (FAILED(hr)) return false;

    hr = audioClient_->GetMixFormat(&waveFormat_);
    if (FAILED(hr)) return false;

    // Initialize in shared mode
    hr = audioClient_->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                  AUDCLNT_STREAMFLAGS_LOOPBACK,
                                  0, 0, waveFormat_, nullptr);
    if (FAILED(hr)) return false;

    hr = audioClient_->GetService(IID_PPV_ARGS(&captureClient_));
    if (FAILED(hr)) return false;

    return true;
}

void WasapiAudioSource::Start()
{
    capturing_ = true;
    audioClient_->Start();
    std::thread(&WasapiAudioSource::CaptureLoop, this).detach();
}

void WasapiAudioSource::Stop()
{
    capturing_ = false;

    if (audioClient_)
    {
        audioClient_->Stop();
    }
}

void WasapiAudioSource::CaptureLoop()
{
    while (capturing_)
    {
        UINT32 packetLength = 0;
        HRESULT hr = captureClient_->GetNextPacketSize(&packetLength);

        while (packetLength != 0)
        {
            BYTE* data;
            UINT32 numFrames; // Number of frames received in the current packet
            DWORD flags;

            hr = captureClient_->GetBuffer(&data, &numFrames, &flags, nullptr, nullptr);
            if (SUCCEEDED(hr))
            {
                // Calculate the number of actual float samples (frames * channels)
                // Assuming mono (1 channel) for simplicity; adjust if stereo.
                size_t numSamples = numFrames; // Assuming mono (1 float per frame)
                float* samples = reinterpret_cast<float*>(data);

                size_t samplesRemaining = numSamples;
                size_t offset = 0;

                while (samplesRemaining > 0)
                {
                    // Calculate how many samples we need to reach the full chunk size
                    size_t needed = chunkSize_ - audioBuffer_.size();

                    // Determine how many samples to copy in this step
                    size_t copyCount = std::min(samplesRemaining, needed);

                    // Append the samples to the buffer
                    audioBuffer_.insert(audioBuffer_.end(), samples + offset, samples + offset + copyCount);

                    // Update counters
                    samplesRemaining -= copyCount;
                    offset += copyCount;

                    // Check if a full chunk is ready
                    if (audioBuffer_.size() == chunkSize_)
                    {
                        // 1. Efficiently move data from the instance buffer to a local, temporary buffer.
                        // This makes 'audioBuffer_' ready for immediate reuse (O(1) move).
                        AudioChunk completeChunk = std::move(audioBuffer_);

                        // 2. Notify ALL observers using the local buffer.
                        // The Notify method will now call PushData(const AudioChunk&),
                        // which forces a COPY into *each* observer's queue.
                        this->Notify(completeChunk);

                        // 3. Reset the instance buffer (already done by std::move and clear)
                        audioBuffer_.clear();
                        audioBuffer_.reserve(chunkSize_);
                    }
                }

                captureClient_->ReleaseBuffer(numFrames);
            }

            // Check for the next packet size
            captureClient_->GetNextPacketSize(&packetLength);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
