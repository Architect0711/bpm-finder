//
// Created by Robert on 2025-09-25.
//

#pragma once
#include "../../audio/IAudioSource.h"
#include "core/CopySink.h"
#include <fstream>

#include "BinFileSink.h"

namespace bpmfinder::files::bin
{
    class AudioBinFileSink : public BinFileSink<audio::AudioChunk>
    {
    public:
        explicit AudioBinFileSink(const std::string& filename) : BinFileSink(filename)
        {
        }

    protected:
        void Process(const audio::AudioChunk data) override
        {
            // std::cout << "Writing " << data.size() << " samples to file " << filename_ << "..." << std::endl;
            file_.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(float));
            ++write_count_;
            std::cout << "File " << filename_ << " size: " << file_.tellp() << " after " << GetWrittenCount() <<
                " entries" << std::endl;
        }
    };
}
