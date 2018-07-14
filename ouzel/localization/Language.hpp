// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace ouzel
{
    class Language final
    {
    public:
        Language() {}
        Language(const std::string& filename);
        Language(const std::vector<uint8_t>& data);

        std::string getString(const std::string& str);

    private:
        std::map<std::string, std::string> strings;
    };
}
