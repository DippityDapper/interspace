#pragma once

#include <cstdint>
#include <vector>

#include "game/world/TileType.hpp"

namespace Game
{
    class TileServer
    {
    public:
        TileType type = GRASS_1;

    public:
        explicit TileServer(TileType _type);
        std::vector<uint8_t> Serialize();
    };
}
