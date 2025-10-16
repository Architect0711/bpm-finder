//
// Created by Robert on 2025-10-16.
//

#pragma once
#include "CopySink.h"
#include "CopyObservable.h"

namespace bpmfinder::core
{
    // Consumes data and emits data
    template <typename InputType, typename OutputType>
    class CopyStage : public CopySink<InputType>, public CopyObservable<OutputType>
    {
        // Funny enough, the sink implementation already does everything we need, so we just need to inherit from
        // CopyObservable and notify the observers there after applying whatever dsp algorithm is applied in this stage
    };
}
