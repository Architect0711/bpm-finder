//
// Created by Robert on 2025-09-24.
//

#pragma once
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <wrl/client.h> // Microsoft::WRL::ComPtr smart pointers
#include <fstream>

namespace bpmfinder::audio
{
    class WasapiAudioSource
    {
    public:
        WasapiAudioSource();
        ~WasapiAudioSource();

        bool Initialize();
        void Start();
        void Stop();

        void CaptureLoop();

    private:
        Microsoft::WRL::ComPtr<IMMDeviceEnumerator> deviceEnumerator_;
        Microsoft::WRL::ComPtr<IMMDevice> device_;
        Microsoft::WRL::ComPtr<IAudioClient> audioClient_;
        Microsoft::WRL::ComPtr<IAudioCaptureClient> captureClient_;
        static void PrintWaveformASCII(const float* samples, size_t numSamples);

        std::ofstream csvFile_;
        WAVEFORMATEX* waveFormat_;
        HANDLE eventHandle_;
        bool capturing_;
    };
}
