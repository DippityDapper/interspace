#pragma once

#include <map>
#include <vector>
#include <memory>

#include "TileType.hpp"

namespace Engine
{
    class Sprite;
}

namespace Game
{
    class Tile;

    class Tiles
    {
    public:
        static std::map<TileType, std::unique_ptr<Tile>> tiles;
        static std::vector<std::unique_ptr<Tile>> uniqueTiles;

    public:
        static void InitRegistry();
        static void RegisterTile(TileType tileType, Tile* tile);
        static Tile* GetTile(TileType tileType);
        static Tile* GetTileUnique(TileType tileType);
    };
}
