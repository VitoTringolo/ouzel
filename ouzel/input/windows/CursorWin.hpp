// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_NATIVECURSORWIN_HPP
#define OUZEL_INPUT_NATIVECURSORWIN_HPP

#include <cstdint>
#include <vector>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#undef NOMINMAX

#include "input/Cursor.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    namespace input
    {
        class CursorWin final
        {
        public:
            explicit CursorWin(SystemCursor systemCursor);
            CursorWin(const std::vector<uint8_t>& data,
                            const Size2<float>& size,
                            graphics::PixelFormat pixelFormat,
                            const Vector2<float>& hotSpot);
            ~CursorWin();

            HCURSOR getCursor() const { return cursor; }

        private:
            HCURSOR cursor = nullptr;
            HCURSOR ownedCursor = nullptr;
            HDC dc = nullptr;
            HBITMAP color = nullptr;
            HBITMAP mask = nullptr;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_NATIVECURSORWIN_HPP
