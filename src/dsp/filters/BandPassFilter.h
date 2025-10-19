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
        BandPassFilter(int cutoff_low, int cutoff_high, int sample_rate, float gain = 1.0f);
        ~BandPassFilter() = default;

        float Process(float sample);

    private:
        void CalculateCoefficients();

        int f1_cutoff_low_;
        int f2_cutoff_high_;
        int fs_sample_rate_;
        float g_gain_;
        float b0_{};
        float b1_{};
        float b2_{};
        float a1_{};
        float a2_{};
        float x1_;
        float x2_;
        float y1_;
        float y2_;
    };
}
