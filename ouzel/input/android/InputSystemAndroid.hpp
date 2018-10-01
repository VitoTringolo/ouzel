// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <jni.h>
#include "input/InputSystem.hpp"
#include "input/android/GamepadDeviceAndroid.hpp"
#include "input/android/MouseDeviceAndroid.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemAndroid: public InputSystem
        {
        public:
            static Keyboard::Key convertKeyCode(jint keyCode);

            InputSystemAndroid();
            virtual ~InputSystemAndroid() {}

            KeyboardDevice* getKeyboardDevice() const { return keyboardDevice; }
            MouseDevice* getMouseDevice() const { return mouseDevice; }
            TouchpadDevice* getTouchpadDevice() const { return touchpadDevice; }

        private:
            uint32_t lastDeviceId = 0;
            KeyboardDevice* keyboardDevice = nullptr;
            MouseDeviceAndroid* mouseDevice = nullptr;
            TouchpadDevice* touchpadDevice = nullptr;
        };
    }
}