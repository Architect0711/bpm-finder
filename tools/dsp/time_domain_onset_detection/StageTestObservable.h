//
// Created by Robert on 2025-10-22.
//

#pragma once

#include "audio/IAudioSource.h"
#include "core/CopyObservable.h"

namespace bpmfinder::tools::dsp::time_domain_onset_detection
{
    // Observer class to capture stage output
    template <typename T>
    class StageTestObservable : public core::CopyObservable<T>
    {
    public:
        void Publish(const T& data)
        {
            this->Notify(data);
        }
    };
}

