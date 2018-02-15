// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <dispatch/dispatch.h>
#include "core/Engine.hpp"

namespace ouzel
{
    class EngineIOS: public Engine
    {
    public:
        EngineIOS(int initArgc, char* initArgv[]);

        virtual int run() override;

        virtual void executeOnMainThread(const std::function<void(void)>& func) override;

        virtual bool openURL(const std::string& url) override;

        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled) override;

    protected:
        dispatch_queue_t mainQueue;

        int argc = 0;
        char** argv = nullptr;
    };
}
