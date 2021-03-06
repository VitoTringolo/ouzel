// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <iterator>
#include <stdexcept>
#include "PCMSound.hpp"
#include "Audio.hpp"
#include "mixer/Stream.hpp"
#include "mixer/Source.hpp"

namespace ouzel
{
    namespace audio
    {
        class PCMData;

        class PCMSource: public mixer::Stream
        {
        public:
            PCMSource(PCMData& pcmData);

            void reset() override
            {
                position = 0;
            }

            void getData(uint32_t frames, std::vector<float>& samples) override;

        private:
            uint32_t position = 0;
        };

        class PCMData: public mixer::Source
        {
        public:
            PCMData(uint16_t initChannels, uint32_t initSampleRate,
                    const std::vector<float>& initSamples):
                samples(initSamples)
            {
                channels = initChannels;
                sampleRate = initSampleRate;
            }

            const std::vector<float>& getSamples() const { return samples; }

            std::unique_ptr<mixer::Stream> createStream() override
            {
                return std::unique_ptr<mixer::Stream>(new PCMSource(*this));
            }

        private:
            std::vector<float> samples;
        };

        PCMSource::PCMSource(PCMData& pcmData):
            Stream(pcmData)
        {
        }

        void PCMSource::getData(uint32_t frames, std::vector<float>& samples)
        {
            uint32_t neededSize = frames * source.getChannels();
            samples.resize(neededSize);

            PCMData& pcmData = static_cast<PCMData&>(source);
            const std::vector<float>& data = pcmData.getSamples();

            uint32_t totalSize = 0;

            while (neededSize > 0)
            {
                if (isRepeating() && (data.size() - position) == 0) reset();

                if (data.size() - position < neededSize)
                {
                    std::copy(data.begin() + position,
                              data.end(),
                              samples.begin() + totalSize);
                    totalSize += static_cast<uint32_t>(data.size() - position);
                    neededSize -= static_cast<uint32_t>(data.size() - position);
                    position = static_cast<uint32_t>(data.size());
                }
                else
                {
                    std::copy(data.begin() + position,
                              data.begin() + position + neededSize,
                              samples.begin() + totalSize);
                    totalSize += neededSize;
                    position += neededSize;
                    neededSize = 0;
                }

                if (!isRepeating()) break;
            }

            if ((data.size() - position) == 0)
            {
                if (!isRepeating()) playing = false; // TODO: fire event
                reset();
            }

            std::fill(samples.begin() + totalSize, samples.end(), 0.0F);
        }

        PCMSound::PCMSound(Audio& initAudio, uint16_t channels, uint32_t sampleRate,
                           const std::vector<float>& samples):
            Sound(initAudio, initAudio.initSource([channels, sampleRate, samples](){
                return std::unique_ptr<mixer::Source>(new PCMData(channels, sampleRate, samples));
            }))
        {
        }
    } // namespace audio
} // namespace ouzel
