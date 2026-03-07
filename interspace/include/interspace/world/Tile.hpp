#pragma once

#include <cstdint>
#include <string>

namespace Interspace
{
    class Tile
    {
      public:
        uint32_t tileId = 0;
        uint32_t variant = 0;
        std::string tileName{};
        bool walkable = true;
    };
}