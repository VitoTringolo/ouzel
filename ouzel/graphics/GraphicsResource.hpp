// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_GRAPHICSRESOURCE_HPP
#define OUZEL_GRAPHICS_GRAPHICSRESOURCE_HPP

#include <cstdint>

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class Resource final
        {
        public:
            Resource() {}
            Resource(Renderer& initRenderer);
            ~Resource();

            Resource(const Resource&) = delete;
            Resource& operator=(const Resource&) = delete;

            Resource(Resource&& other):
                renderer(other.renderer),
                id(other.id)
            {

                other.renderer = nullptr;
                other.id = 0;
            }

            Resource& operator=(Resource&& other)
            {
                if (&other != this)
                {
                    renderer = other.renderer;
                    id = other.id;
                    other.renderer = nullptr;
                    other.id = 0;
                }

                return *this;
            }

            Renderer* getRenderer() const
            {
                return renderer;
            }

            uintptr_t getId() const
            {
                return id;
            }

        private:
            Renderer* renderer = nullptr;
            uintptr_t id = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_GRAPHICSRESOURCE_HPP
