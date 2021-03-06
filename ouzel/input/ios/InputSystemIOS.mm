// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "InputSystemIOS.hpp"
#include "GamepadDeviceIOS.hpp"
#include "core/Engine.hpp"
#include "core/ios/NativeWindowIOS.hpp"
#include "utils/Log.hpp"

@interface ConnectDelegate: NSObject
{
    ouzel::input::InputSystemIOS* input;
}

-(void)handleControllerConnected:(NSNotification*)notification;
-(void)handleControllerDisconnected:(NSNotification*)notification;

@end

@implementation ConnectDelegate

-(id)initWithInput:(ouzel::input::InputSystemIOS*)initInput
{
    if (self = [super init])
        input = initInput;

    return self;
}

-(void)handleControllerConnected:(NSNotification*)notification
{
    input->handleGamepadConnected(notification.object);
}

-(void)handleControllerDisconnected:(NSNotification*)notification
{
    input->handleGamepadDisconnected(notification.object);
}

@end

namespace ouzel
{
    namespace input
    {
        InputSystemIOS::InputSystemIOS(const std::function<std::future<bool>(const Event&)>& initCallback):
            InputSystem(initCallback),
            keyboardDevice(new KeyboardDevice(*this, ++lastDeviceId)),
            touchpadDevice(new TouchpadDevice(*this, ++lastDeviceId, true))
        {
            connectDelegate = [[ConnectDelegate alloc] initWithInput:this];

            [[NSNotificationCenter defaultCenter] addObserver:connectDelegate
                                                     selector:@selector(handleControllerConnected:)
                                                         name:GCControllerDidConnectNotification
                                                       object:nil];

            [[NSNotificationCenter defaultCenter] addObserver:connectDelegate
                                                     selector:@selector(handleControllerDisconnected:)
                                                         name:GCControllerDidDisconnectNotification
                                                       object:nil];

            for (GCController* controller in [GCController controllers])
                handleGamepadConnected(controller);

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        InputSystemIOS::~InputSystemIOS()
        {
            if (connectDelegate)
            {
                [[NSNotificationCenter defaultCenter] removeObserver:connectDelegate];

                [connectDelegate release];
            }
        }

        void InputSystemIOS::executeCommand(const Command& command)
        {
            switch (command.type)
            {
                case Command::Type::START_DEVICE_DISCOVERY:
                    startGamepadDiscovery();
                    break;
                case Command::Type::STOP_DEVICE_DISCOVERY:
                    stopGamepadDiscovery();
                    break;
                case Command::Type::SET_PLAYER_INDEX:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        GamepadDeviceIOS* gamepadDevice = static_cast<GamepadDeviceIOS*>(inputDevice);
                        gamepadDevice->setPlayerIndex(command.playerIndex);
                    }
                    break;
                }
                case Command::Type::SET_VIBRATION:
                {
                    break;
                }
                case Command::Type::SHOW_VIRTUAL_KEYBOARD:
                    showVirtualKeyboard();
                    break;
                case Command::Type::HIDE_VIRTUAL_KEYBOARD:
                    hideVirtualKeyboard();
                    break;
                default:
                    break;
            }
        }

        void InputSystemIOS::startGamepadDiscovery()
        {
            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        void InputSystemIOS::stopGamepadDiscovery()
        {
            [GCController stopWirelessControllerDiscovery];
        }

        void InputSystemIOS::handleGamepadDiscoveryCompleted()
        {
            sendEvent(Event(Event::Type::DEVICE_DISCOVERY_COMPLETE));
        }

        void InputSystemIOS::handleGamepadConnected(GCControllerPtr controller)
        {
            std::vector<int32_t> playerIndices = {0, 1, 2, 3};

            for (const auto& i : gamepadDevices)
            {
                auto n = std::find(playerIndices.begin(), playerIndices.end(), i.second->getPlayerIndex());
                if (n != playerIndices.end()) playerIndices.erase(n);
            }

            if (!playerIndices.empty()) controller.playerIndex = static_cast<GCControllerPlayerIndex>(playerIndices.front());

            std::unique_ptr<GamepadDeviceIOS> gamepadDevice(new GamepadDeviceIOS(*this, ++lastDeviceId, controller));
            gamepadDevices.insert(std::make_pair(controller, std::move(gamepadDevice)));
        }

        void InputSystemIOS::handleGamepadDisconnected(GCControllerPtr controller)
        {
            auto i = gamepadDevices.find(controller);

            if (i != gamepadDevices.end())
                gamepadDevices.erase(i);
        }

        void InputSystemIOS::showVirtualKeyboard()
        {
            NativeWindowIOS* windowIOS = static_cast<NativeWindowIOS*>(engine->getWindow()->getNativeWindow());
            UITextField* textField = windowIOS->getTextField();
            [textField becomeFirstResponder];
        }

        void InputSystemIOS::hideVirtualKeyboard()
        {
            NativeWindowIOS* windowIOS = static_cast<NativeWindowIOS*>(engine->getWindow()->getNativeWindow());
            UITextField* textField = windowIOS->getTextField();
            [textField resignFirstResponder];
        }
    } // namespace input
} // namespace ouzel
