#pragma once

#include <map>
#include <memory>
#include <string>

namespace Game
{
    struct FactionData
    {
        uint16_t id = 0;
        std::string name;
        uint32_t ownerId = 0;
        std::map<uint32_t, std::string> members{};
    };
}
