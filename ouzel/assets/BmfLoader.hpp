// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ASSETS_BMFLOADER_HPP
#define OUZEL_ASSETS_BMFLOADER_HPP

#include "assets/Loader.hpp"

namespace ouzel
{
    namespace assets
    {
        class BmfLoader final: public Loader
        {
        public:
            static constexpr uint32_t TYPE = Loader::FONT;

            explicit BmfLoader(Cache& initCache);
            bool loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps = true) override;
        };
    } // namespace assets
} // namespace ouzel

#endif // OUZEL_ASSETS_BMFLOADER_HPP
