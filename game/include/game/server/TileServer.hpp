#pragma once

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
    };
}
