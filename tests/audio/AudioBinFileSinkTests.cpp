//
// Created by Robert on 2025-10-05.
//

#include <gtest/gtest.h>
#include "../../src/files/bin/AudioBinFileSink.h"
#include <fstream>

using namespace bpmfinder::audio;

/**
 * @class TestAudioBinFileSink
 *
 * @brief A class responsible for testing writing the audio data into a CSV file format.
 *
 * Exposes the Process publicly to enable testing.
 */
class TestAudioBinFileSink : public AudioBinFileSink
{
public:
    explicit TestAudioBinFileSink(const std::string& filename) : AudioBinFileSink(filename)
    {
    }

    using AudioBinFileSink::Process;
};

TEST(AudioBinFileSinkTests, WhenProcessingAudioChunk_ThenDataIsWrittenToFile)
{
    // -------------------- Arrange --------------------
    const std::string filename = "test_audio.bin";
    AudioChunk testData = {1.0f, 2.0f, 3.0f, 4.0f};

    // -------------------- Act ------------------------
    {
        TestAudioBinFileSink sink(filename);
        sink.Process(testData);
    } // sink destructor called here, file is closed and flushed

    // -------------------- Assert ---------------------
    std::ifstream file(filename, std::ios::binary);
    ASSERT_TRUE(file.is_open());

    std::vector<float> readData(testData.size());
    file.read(reinterpret_cast<char*>(readData.data()), testData.size() * sizeof(float));

    EXPECT_EQ(readData, testData);

    file.close();
}


