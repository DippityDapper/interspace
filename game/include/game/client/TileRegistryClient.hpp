#pragma once

#include <map>
#include <memory>
#include <vector>

#include "game/client/TileClient.hpp"

namespace Game
{
    class TileRegistryClient
    {
    public:
        static inline std::map<TileType, std::unique_ptr<TileClient>> tiles;
        static inline std::vector<std::unique_ptr<TileClient>> uniqueTiles;

    public:
        static void InitRegistry();
        static void RegisterTile(TileType tileType, TileClient* tile);
        static TileClient* GetTile(TileType tileType);
        static std::map<TileType, TileClient*> GetTiles();
    };
}
