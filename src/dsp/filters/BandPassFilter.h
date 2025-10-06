//
// Created by Robert on 2025-10-07.
//

#pragma once
#include <vector>

namespace bpmfinder::dsp::filters
{
    class BandPassFilter
    {
    public:
        BandPassFilter(float cutoff_low, float cutoff_high, float sample_rate, float gain = 1.0f);
        ~BandPassFilter() = default;

        void Process(float sample);

    private:
        void CalculateCoefficients();

        float f1_cutoff_low_;
        float f2_cutoff_high_;
        float fs_sample_rate_;
        float g_gain_;
        float b0_;
        float b1_;
        float b2_;
        float a1_;
        float a2_;
        float x1_;
        float x2_;
        float y1_;
        float y2_;
    };
}
