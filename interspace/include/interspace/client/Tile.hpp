#pragma once

#include "igneous/Sprite.hpp"
#include "interspace/world/TileData.hpp"

namespace Interspace::Client
{
    class Tile
    {
    public:
        TileData data{};

        std::string texturePath{};
        uint32_t atlasWidth = 0;
        uint32_t atlasHeight = 0;
        uint32_t atlasX = 0;
        uint32_t atlasY = 0;
    };
}
