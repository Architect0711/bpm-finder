//
// Created by Robert on 2025-09-25.
//

#pragma once

#include <vector>
#include "core/Observable.h"

namespace bpmfinder::audio
{
    using AudioChunk = std::vector<float>;

    class IAudioSource : public core::Observable<AudioChunk>
    {
    public:
        virtual ~IAudioSource() = default;

        // Initialize the source (WASAPI, file, etc.)
        virtual bool Initialize() = 0;

        // Start/stop capture or playback
        virtual void Start() = 0;
        virtual void Stop() = 0;
    };
}
