#pragma once

#include <string>
#include <cstdint>

#include "igneous/Vec2.hpp"

namespace Interspace
{
    struct TileData
    {
        uint32_t tileId = 0;
        uint32_t variant = 0;
        std::string tileName{};
        bool walkable = true;
    };
}
