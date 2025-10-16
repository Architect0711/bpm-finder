//
// Created by Robert on 2025-10-07.
//

#include "BandPassFilter.h"
#include "audio/IAudioSource.h"
#include <cmath>
#include <algorithm> // For std::clamp if you want to bound inputs

namespace bpmfinder::dsp::filters
{
    BandPassFilter::BandPassFilter(const float cutoff_low, const float cutoff_high, const float sample_rate,
                                   const float gain) :
        f1_cutoff_low_(cutoff_low),
        f2_cutoff_high_(cutoff_high),
        fs_sample_rate_(sample_rate),
        g_gain_(gain),
        // The state history is initialized with zeros, representing "silence before the audio started"
        x1_(0.0f),
        x2_(0.0f),
        y1_(0.0f),
        y2_(0.0f)
    {
        CalculateCoefficients();
    }

    void BandPassFilter::CalculateCoefficients()
    {
        // --- Step 2: Calculate Derived Parameters ---
        const float fc = std::sqrt(f1_cutoff_low_ * f2_cutoff_high_);
        const float B = f2_cutoff_high_ - f1_cutoff_low_;
        const float Q = fc / B;

        // --- Step 3: Calculate Digital Intermediate Variables ---
        const float omega0 = 2.0f * M_PI * fc / fs_sample_rate_; // M_PI is math constant
        const float alpha = std::sin(omega0) / (2.0f * Q);

        // --- Step 4: Calculate Raw Coefficients ---
        const float b0_raw = alpha * g_gain_; // Apply gain (though typically applied after normalization)
        constexpr float b1_raw = 0.0f;
        const float b2_raw = -alpha * g_gain_; // Apply gain
        const float a0_raw = 1.0f + alpha;
        const float a1_raw = -2.0f * std::cos(omega0);
        const float a2_raw = 1.0f - alpha;

        // --- Step 5: Calculate Final Normalized Coefficients ---
        // The filter equation uses: y[n] = b0*x[n] + ... - a1*y[n-1] - ...
        // Note the sign inversion for the feedback 'a' coefficients in the formula vs. implementation
        b0_ = b0_raw / a0_raw;
        b1_ = b1_raw / a0_raw;
        b2_ = b2_raw / a0_raw;

        // The coefficients passed to the filter should be the NEGATIVE of the normalized 'a' values
        // IF your filter's difference equation is written as: ... - a1*y[n-1] ...
        a1_ = a1_raw / a0_raw; // The difference equation handles the subtraction
        a2_ = a2_raw / a0_raw;
    }

    float BandPassFilter::Process(float sample)
    {
        // The Difference Equation:
        // y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
        float output = b0_ * sample + b1_ * x1_ + b2_ * x2_ - a1_ * y1_ - a2_ * y2_;

        // Update the state history for the next sample
        x2_ = x1_;
        x1_ = sample;
        y2_ = y1_;
        y1_ = output;

        return output;
    }
}
