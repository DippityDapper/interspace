#pragma once

#include "igneous/resources/Sprite.hpp"

#include <cstdint>
#include <string>

namespace Interspace
{
    struct TileData
    {
      public:
        uint32_t tileId = 0;
        uint32_t variant = 0;
        std::string tileName{};
        bool walkable = true;
        std::unique_ptr<Engine::Sprite> sprite = nullptr;

        void Set(const TileData& newTile)
        {
            if (tileId != newTile.tileId)
                tileId = newTile.tileId;
            if (variant != newTile.variant)
                variant = newTile.variant;
            if (tileName != newTile.tileName)
                tileName = newTile.tileName;
            if (walkable != newTile.walkable)
                walkable = newTile.walkable;
        }
    };
}