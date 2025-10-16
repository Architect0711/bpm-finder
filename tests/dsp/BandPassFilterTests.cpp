//
// Created by Robert on 2025-10-16.
//

#include <gtest/gtest.h>
#include "../../src/dsp/filters/BandPassFilter.h"
#include <cmath>
#include <vector>

using namespace bpmfinder::dsp::filters;


// ============================================================================
// Test Fixture
// ============================================================================

class BandPassFilterTests : public ::testing::Test
{
protected:
    float sampleRate = 48000.0f;
    float lowCutoff = 100.0f;
    float highCutoff = 10000.0f;
    int chunkSize = 1024;
    BandPassFilter filter_ = BandPassFilter(lowCutoff, highCutoff, sampleRate);

    void SetUp() override
    {
        InitFilter();
    }

    void InitFilter()
    {
        filter_ = BandPassFilter(lowCutoff, highCutoff, sampleRate);
    }

    void TearDown() override
    {
    }

    // Helper function to calculate RMS (Root Mean Square) of a signal
    static float CalculateRMS(const std::vector<float>& signal, const size_t startIdx, const size_t endIdx)
    {
        if (startIdx >= endIdx || endIdx > signal.size())
        {
            return 0.0f;
        }

        float sum = 0.0f;
        for (size_t i = startIdx; i < endIdx; ++i)
        {
            sum += signal[i] * signal[i];
        }

        return std::sqrt(sum / static_cast<float>(endIdx - startIdx));
    }
};

TEST_F(BandPassFilterTests, PassesFrequencyInPassband)
{
    // -------------------- Arrange --------------------
    float frequency = 1000.0f;
    // Generate a 1kHz sine wave (should pass through)
    std::vector<float> output;

    // -------------------- Act ------------------------
    for (int i = 0; i < chunkSize; ++i)
    {
        const float sample = std::sin(2.0f * M_PI * frequency * i / sampleRate);
        output.push_back(filter_.Process(sample));
    }

    // -------------------- Assert ---------------------
    // After settling, output should have similar amplitude
    // (check last ~500 samples to skip transient response)
    float avgAmplitude = CalculateRMS(output, 500, chunkSize);

    // Expect amplitude to be significant (> 0.5) but not necessarily 1.0
    EXPECT_GT(avgAmplitude, 0.5f); // Should pass frequencies in passband
    EXPECT_LT(avgAmplitude, 1.5f); // Sanity check - shouldn't amplify much
}

TEST_F(BandPassFilterTests, AttenuatesFrequencyBelowPassband)
{
    // -------------------- Arrange --------------------
    float frequency = 50.0f;
    // Generate a 1kHz sine wave (should pass through)
    std::vector<float> output;

    // -------------------- Act ------------------------
    for (int i = 0; i < chunkSize; ++i)
    {
        const float sample = std::sin(2.0f * M_PI * frequency * i / sampleRate);
        output.push_back(filter_.Process(sample));
    }

    // -------------------- Assert ---------------------
    // After settling, output should have similar amplitude
    // (check last ~500 samples to skip transient response)
    float avgAmplitude = CalculateRMS(output, 500, chunkSize);
    EXPECT_LT(avgAmplitude, 0.4f);
}

TEST_F(BandPassFilterTests, ImpulseResponseSettles)
{
    BandPassFilter filter(100.0f, 10000.0f, 48000.0f);

    // Send an impulse (1.0) followed by silence
    float impulseResponse = filter.Process(1.0f);
    std::vector<float> response;
    response.reserve(1000);
    for (int i = 0; i < 1000; ++i)
    {
        response.push_back(filter.Process(0.0f));
    }

    // Filter should eventually settle to near-zero
    EXPECT_NEAR(response.back(), 0.0f, 0.001f);
}

TEST(BandPassFilterTest, BlocksDCSignal)
{
    BandPassFilter filter(100.0f, 10000.0f, 48000.0f);

    // Feed constant 1.0 (DC)
    std::vector<float> output;
    output.reserve(1000);
    for (int i = 0; i < 1000; ++i)
    {
        output.push_back(filter.Process(1.0f));
    }

    // Output should decay to near-zero
    EXPECT_NEAR(output.back(), 0.0f, 0.01f);
}
