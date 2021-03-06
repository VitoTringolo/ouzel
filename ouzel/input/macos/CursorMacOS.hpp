// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_NATIVECURSORMACOS_HPP
#define OUZEL_INPUT_NATIVECURSORMACOS_HPP

#include <cstdint>
#include <vector>

#if defined(__OBJC__)
#  import <AppKit/AppKit.h>
typedef NSCursor* NSCursorPtr;
#else
#  include <objc/objc.h>
typedef id NSCursorPtr;
#endif

#include "input/Cursor.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    namespace input
    {
        class CursorMacOS final
        {
        public:
            explicit CursorMacOS(SystemCursor systemCursor);
            CursorMacOS(const std::vector<uint8_t>& newData,
                              const Size2<float>& size,
                              graphics::PixelFormat pixelFormat,
                              const Vector2<float>& hotSpot);
            ~CursorMacOS();

            inline NSCursorPtr getCursor() const { return cursor; }

        private:
            NSCursorPtr cursor = nil;
            std::vector<uint8_t> data;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_NATIVECURSORMACOS_HPP
