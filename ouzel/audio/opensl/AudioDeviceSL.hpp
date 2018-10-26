// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENSL

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDeviceSL final: public AudioDevice
        {
            friend Audio;
        public:
            virtual ~AudioDeviceSL();

            void enqueue(SLAndroidSimpleBufferQueueItf bufferQueue);

            SLEngineItf getEngine() const { return engine; }
            SLObjectItf getOutputMix() const { return outputMixObject; }

        protected:
            AudioDeviceSL();

        private:
            SLObjectItf engineObject = nullptr;
            SLEngineItf engine = nullptr;
            SLObjectItf outputMixObject = nullptr;

            SLObjectItf playerObject = nullptr;
            SLPlayItf player = nullptr;
            SLAndroidSimpleBufferQueueItf bufferQueue = nullptr;
            SLVolumeItf playerVolume = nullptr;

            std::vector<uint8_t> data;
        };
    } // namespace audio
} // namespace ouzel

#endif
