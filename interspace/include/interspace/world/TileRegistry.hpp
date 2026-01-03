#pragma once

#include <map>
#include <memory>

#include "interspace/world/TileData.hpp"

namespace Interspace
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
