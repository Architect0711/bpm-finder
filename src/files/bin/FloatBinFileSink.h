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
        explicit FloatBinFileSink(const std::string& filename) : BinFileSink(filename)
        {
        }

    protected:
        void Process(float data) override
        {
            // std::cout << "Writing " << data << " energy value to file " << filename_ << "..." << std::endl;
            file_.write(reinterpret_cast<const char*>(&data), sizeof(float));
            ++write_count_;
            std::cout << "File " << filename_ << " size: " << file_.tellp() << " after " << GetWrittenCount() <<
                " entries" << std::endl;
        }
    };
}
