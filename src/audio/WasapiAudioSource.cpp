//
// Created by Robert on 2025-09-24.
//

#include "WasapiAudioSource.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <combaseapi.h>

using namespace bpmfinder::audio;

WasapiAudioSource::WasapiAudioSource()
    : waveFormat_(nullptr), eventHandle_(nullptr), capturing_(false)
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
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
    csvFile_.open("waveform.csv", std::ios::out); // overwrite each run
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

    if (csvFile_.is_open())
    {
        csvFile_.close();
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
            UINT32 numFrames;
            DWORD flags;

            hr = captureClient_->GetBuffer(&data, &numFrames, &flags, nullptr, nullptr);
            if (SUCCEEDED(hr))
            {
                float* samples = reinterpret_cast<float*>(data);

                PrintWaveformASCII(samples, numFrames);
                for (UINT32 i = 0; i < numFrames; i++)
                {
                    csvFile_ << samples[i] << "\n";
                }
                captureClient_->ReleaseBuffer(numFrames);
            }

            captureClient_->GetNextPacketSize(&packetLength);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void WasapiAudioSource::PrintWaveformASCII(const float* samples, size_t numSamples)
{
    const int width = 50; // width of the graph
    for (size_t i = 0; i < numSamples; i++)
    {
        int pos = static_cast<int>((samples[i] + 1.0f) * 0.5f * width);
        for (int j = 0; j < width; j++)
        {
            if (j == pos)
                std::cout << "*"; // waveform point
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }
}
