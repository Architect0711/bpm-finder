//
// Created by Robert on 2025-09-24.
//

#pragma once
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <wrl/client.h> // Microsoft::WRL::ComPtr smart pointers
#include <fstream>

#include "IAudioSource.h"

namespace bpmfinder::audio
{
    class WasapiAudioSource final : public IAudioSource
    {
    public:
        explicit WasapiAudioSource(size_t chunkSize);
        ~WasapiAudioSource() override;

        bool Initialize() override;
        void Start() override;
        void Stop() override;

        void CaptureLoop();

        uint32_t GetSampleRate() const { return waveFormat_ ? waveFormat_->nSamplesPerSec : 0; }
        uint16_t GetNumChannels() const { return waveFormat_ ? waveFormat_->nChannels : 0; }

    private:
        Microsoft::WRL::ComPtr<IMMDeviceEnumerator> deviceEnumerator_;
        Microsoft::WRL::ComPtr<IMMDevice> device_;
        Microsoft::WRL::ComPtr<IAudioClient> audioClient_;
        Microsoft::WRL::ComPtr<IAudioCaptureClient> captureClient_;

        AudioChunk audioBuffer_;
        size_t chunkSize_;
        WAVEFORMATEX* waveFormat_;
        HANDLE eventHandle_;
        bool capturing_;
        uint32_t sampleRate_ = 0;
        uint16_t numChannels_ = 0;
    };
}
