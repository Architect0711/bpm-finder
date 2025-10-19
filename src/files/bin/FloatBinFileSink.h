//
// Created by Robert on 2025-10-17.
//

#pragma once
#include "../../audio/IAudioSource.h"
#include "core/CopySink.h"
#include <fstream>

namespace bpmfinder::files::bin
{
    class FloatBinFileSink : public BinFileSink<float>
    {
    public:
        explicit FloatBinFileSink(const std::string& filename) : BinFileSink(
            filename, logging::LoggerFactory::GetLogger("FloatBinFileSink"))
        {
        }

    protected:
        void Process(float data) override
        {
            file_.write(reinterpret_cast<const char*>(&data), sizeof(float));
            ++write_count_;
            logger_->debug("File {} size: {} after {} entries", filename_, static_cast<std::streamoff>(file_.tellp())
                           , GetWrittenCount());
        }
    };
}
