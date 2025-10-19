//
// Created by Robert on 2025-09-25.
//

#pragma once
#include "../../audio/IAudioSource.h"
#include "core/CopySink.h"
#include <fstream>
#include "BinFileSink.h"
#include "logging/LoggerFactory.h"

namespace bpmfinder::files::bin
{
    class AudioBinFileSink : public BinFileSink<audio::AudioChunk>
    {
    public:
        explicit AudioBinFileSink(const std::string& filename) : BinFileSink(
            filename, logging::LoggerFactory::GetLogger("AudioBinFileSink"))
        {
        }

    protected:
        void Process(const audio::AudioChunk data) override
        {
            file_.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(float));
            ++write_count_;
            logger_->debug("File {} size: {} after {} entries", filename_, static_cast<std::streamoff>(file_.tellp()),
                           GetWrittenCount());
        }
    };
}
