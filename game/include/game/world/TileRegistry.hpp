#pragma once

#include <map>
#include <memory>

#include "game/world/TileData.hpp"

namespace Game
{
    class TileRegistry
    {
    public:
        static inline std::map<TileType, std::unique_ptr<TileData>> tiles;

    public:
        static void Init();
        static TileData* GetTile(TileType tileType);
    };
}
