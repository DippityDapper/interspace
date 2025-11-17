#pragma once

#include <map>
#include <memory>
#include <vector>

#include "game/server/TileServer.hpp"

namespace Game
{
    class TileRegistryServer
    {
    public:
        static inline std::map<TileType, std::unique_ptr<TileServer>> tiles;
        static inline std::vector<std::unique_ptr<TileServer>> uniqueTiles;

    public:
        static void InitRegistry();
        static void RegisterTile(TileType tileType, TileServer* tile);
        static TileServer* GetTile(TileType tileType);
    };
}