//
// Created by Robert on 2025-10-19.
//

#include "TimeDomainOnsetDetectionResult.h"
#include "audio/IAudioSource.h"
#include "core/CopyStage.h"
#include "logging/LoggerFactory.h"

namespace bpmfinder::dsp::time_domain_onset_detection
{
    class PipelineResultInitializationStage : public core::CopyStage<audio::AudioChunk, TimeDomainOnsetDetectionResult>
    {
    public:
        explicit PipelineResultInitializationStage(const int sampleRate, const int chunkSize,
                                                   const int bandPassLowCutoff,
                                                   const int bandPassHighCutoff,
                                                   const int bandPassGain)
            : sampleRate_(sampleRate),
              chunkSize_(chunkSize),
              bandPassLowCutoff_(bandPassLowCutoff),
              bandPassHighCutoff_(bandPassHighCutoff),
              bandPassGain_(bandPassGain),
              logger_(logging::LoggerFactory::GetLogger("PipelineResultInitializationStage"))
        {
            logger_->info(
                "PipelineResultInitializationStage initialized - Sample Rate: {}s, Chunk Size: {}, Bandpass: {} - {} Hz Gain: {}",
                sampleRate, chunkSize, bandPassLowCutoff, bandPassHighCutoff, bandPassGain);
        }

    protected:
        void Process(const audio::AudioChunk audioChunk) override
        {
            const TimeDomainOnsetDetectionResult result(chunkIndex_, audioChunk, sampleRate_, chunkSize_,
                                                        bandPassLowCutoff_, bandPassHighCutoff_, bandPassGain_);

            ++chunkIndex_;

            this->Notify(result);
        }

    private:
        int sampleRate_;
        int chunkSize_;
        int bandPassLowCutoff_;
        int bandPassHighCutoff_;
        int bandPassGain_;

        int chunkIndex_ = 0;

        std::shared_ptr<spdlog::logger> logger_;
    };
}
