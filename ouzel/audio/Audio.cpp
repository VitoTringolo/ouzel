// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "core/Setup.h"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "Listener.hpp"
#include "alsa/ALSAAudioDevice.hpp"
#include "core/Engine.hpp"
#include "coreaudio/CAAudioDevice.hpp"
#include "dsound/DSAudioDevice.hpp"
#include "empty/EmptyAudioDevice.hpp"
#include "openal/OALAudioDevice.hpp"
#include "opensl/OSLAudioDevice.hpp"
#include "xaudio2/XA2AudioDevice.hpp"
#include "wasapi/WASAPIAudioDevice.hpp"
#include "math/MathUtils.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace audio
    {
        Driver Audio::getDriver(const std::string& driver)
        {
            if (driver.empty() || driver == "default")
            {
                auto availableDrivers = Audio::getAvailableAudioDrivers();

                if (availableDrivers.find(Driver::WASAPI) != availableDrivers.end())
                    return Driver::WASAPI;
                else if (availableDrivers.find(Driver::COREAUDIO) != availableDrivers.end())
                    return Driver::COREAUDIO;
                else if (availableDrivers.find(Driver::ALSA) != availableDrivers.end())
                    return Driver::ALSA;
                else if (availableDrivers.find(Driver::OPENAL) != availableDrivers.end())
                    return Driver::OPENAL;
                else if (availableDrivers.find(Driver::XAUDIO2) != availableDrivers.end())
                    return Driver::XAUDIO2;
                else if (availableDrivers.find(Driver::DIRECTSOUND) != availableDrivers.end())
                    return Driver::DIRECTSOUND;
                else if (availableDrivers.find(Driver::OPENSL) != availableDrivers.end())
                    return Driver::OPENSL;
                else
                    return Driver::EMPTY;
            }
            else if (driver == "empty")
                return Driver::EMPTY;
            else if (driver == "openal")
                return Driver::OPENAL;
            else if (driver == "directsound")
                return Driver::DIRECTSOUND;
            else if (driver == "xaudio2")
                return Driver::XAUDIO2;
            else if (driver == "opensl")
                return Driver::OPENSL;
            else if (driver == "coreaudio")
                return Driver::COREAUDIO;
            else if (driver == "alsa")
                return Driver::ALSA;
            else if (driver == "wasapi")
                return Driver::WASAPI;
            else
                throw std::runtime_error("Invalid audio driver");
        }

        std::set<Driver> Audio::getAvailableAudioDrivers()
        {
            static std::set<Driver> availableDrivers;

            if (availableDrivers.empty())
            {
                availableDrivers.insert(Driver::EMPTY);

#if OUZEL_COMPILE_OPENAL
                availableDrivers.insert(Driver::OPENAL);
#endif

#if OUZEL_COMPILE_DIRECTSOUND
                availableDrivers.insert(Driver::DIRECTSOUND);
#endif

#if OUZEL_COMPILE_XAUDIO2
                availableDrivers.insert(Driver::XAUDIO2);
#endif

#if OUZEL_COMPILE_OPENSL
                availableDrivers.insert(Driver::OPENSL);
#endif

#if OUZEL_COMPILE_COREAUDIO
                availableDrivers.insert(Driver::COREAUDIO);
#endif

#if OUZEL_COMPILE_ALSA
                availableDrivers.insert(Driver::ALSA);
#endif
#if OUZEL_COMPILE_WASAPI
                availableDrivers.insert(Driver::WASAPI);
#endif
            }

            return availableDrivers;
        }

        static std::unique_ptr<AudioDevice> createAudioDevice(Driver driver, mixer::Mixer& mixer, bool debugAudio, Window* window)
        {
            auto dataGetter = std::bind(&mixer::Mixer::getData, &mixer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

            switch (driver)
            {
#if OUZEL_COMPILE_OPENAL
                case Driver::OPENAL:
                    engine->log(Log::Level::INFO) << "Using OpenAL audio driver";
                    return std::unique_ptr<AudioDevice>(new OALAudioDevice(dataGetter));
#endif
#if OUZEL_COMPILE_DIRECTSOUND
                case Driver::DIRECTSOUND:
                    engine->log(Log::Level::INFO) << "Using DirectSound audio driver";
                    return std::unique_ptr<AudioDevice>(new DSAudioDevice(dataGetter, window));
#endif
#if OUZEL_COMPILE_XAUDIO2
                case Driver::XAUDIO2:
                    engine->log(Log::Level::INFO) << "Using XAudio 2 audio driver";
                    return std::unique_ptr<AudioDevice>(new XA2AudioDevice(dataGetter, debugAudio));
#endif
#if OUZEL_COMPILE_OPENSL
                case Driver::OPENSL:
                    engine->log(Log::Level::INFO) << "Using OpenSL ES audio driver";
                    return std::unique_ptr<AudioDevice>(new OSLAudioDevice(dataGetter));
#endif
#if OUZEL_COMPILE_COREAUDIO
                case Driver::COREAUDIO:
                    engine->log(Log::Level::INFO) << "Using CoreAudio audio driver";
                    return std::unique_ptr<AudioDevice>(new CAAudioDevice(dataGetter));
#endif
#if OUZEL_COMPILE_ALSA
                case Driver::ALSA:
                    engine->log(Log::Level::INFO) << "Using ALSA audio driver";
                    return std::unique_ptr<AudioDevice>(new ALSAAudioDevice(dataGetter));
#endif
#if OUZEL_COMPILE_WASAPI
                case Driver::WASAPI:
                    engine->log(Log::Level::INFO) << "Using WASAPI audio driver";
                    return std::unique_ptr<AudioDevice>(new WASAPIAudioDevice(dataGetter));
#endif
                default:
                    engine->log(Log::Level::INFO) << "Not using audio driver";
                    (void)debugAudio;
                    (void)window;
                    return std::unique_ptr<AudioDevice>(new EmptyAudioDevice(dataGetter));
            }
        }

        Audio::Audio(Driver driver, bool debugAudio, Window* window):
            mixer(std::bind(&Audio::eventCallback, this, std::placeholders::_1)),
            masterMix(*this),
            device(createAudioDevice(driver, mixer, debugAudio, window))
        {
            mixer.addCommand(std::unique_ptr<mixer::Command>(new mixer::SetMasterBusCommand(masterMix.getBusId())));
        }

        Audio::~Audio()
        {
        }

        void Audio::update()
        {
            // TODO: handle events from the audio device
        }

        void Audio::deleteObject(uintptr_t objectId)
        {
            mixer.addCommand(std::unique_ptr<mixer::Command>(new mixer::DeleteObjectCommand(objectId)));
        }

        uintptr_t Audio::initBus()
        {
            uintptr_t busId = mixer.getObjectId();
            mixer.addCommand(std::unique_ptr<mixer::Command>(new mixer::InitBusCommand(busId)));
            return busId;
        }

        uintptr_t Audio::initStream(uintptr_t sourceId)
        {
            uintptr_t streamId = mixer.getObjectId();
            mixer.addCommand(std::unique_ptr<mixer::Command>(new mixer::InitStreamCommand(streamId, sourceId)));
            return streamId;
        }

        uintptr_t Audio::initSource(const std::function<std::unique_ptr<mixer::Source>()>& initFunction)
        {
            uintptr_t sourceId = mixer.getObjectId();
            mixer.addCommand(std::unique_ptr<mixer::Command>(new mixer::InitSourceCommand(sourceId,
                                                                                          initFunction)));
            return sourceId;
        }

        uintptr_t Audio::initProcessor(std::unique_ptr<mixer::Processor>&& processor)
        {
            uintptr_t processorId = mixer.getObjectId();
            mixer.addCommand(std::unique_ptr<mixer::Command>(new mixer::InitProcessorCommand(processorId,
                                                                                             std::forward<std::unique_ptr<mixer::Processor>>(processor))));
            return processorId;
        }

        void Audio::updateProcessor(uintptr_t processorId, const std::function<void(mixer::Processor*)>& updateFunction)
        {
            mixer.addCommand(std::unique_ptr<mixer::Command>(new mixer::UpdateProcessorCommand(processorId, updateFunction)));
        }

        void Audio::getData(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)
        {
            mixer.process();
            mixer.getData(frames, channels, sampleRate, samples);
        }

        void Audio::eventCallback(const mixer::Mixer::Event& event)
        {

        }
    } // namespace audio
} // namespace ouzel
