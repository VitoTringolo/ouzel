// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_STREAM_HPP
#define OUZEL_AUDIO_MIXER_STREAM_HPP

#include "audio/mixer/Object.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Bus;
            class Source;

            class Stream: public Object
            {
                friend Bus;
            public:
                Stream(Source& initSource);
                ~Stream();

                const Source& getSource() const { return source; }

                virtual void getData(uint32_t frames, std::vector<float>& samples) = 0;

                void setOutput(Bus* newOutput);

                bool isPlaying() const { return playing; }
                void play(bool repeat);

                bool isRepeating() const { return repeating; }
                void stop(bool shouldReset);
                virtual void reset() = 0;

            protected:
                Source& source;
                Bus* output = nullptr;
                bool playing = false;
                bool repeating = false;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_STREAM_HPP
