#pragma once

#include <map>
#include <vector>
#include <memory>

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
        enum Type
        {
            GRASS_1,
            GRASS_2,
            GRASS_3,
            GRASS_4,
            GRASS_5,
            GRASS_6,
            GRASS_7,
            GRASS_8,
            GRASS_9,
            GRASS_10,
            GRASS_11,
            GRASS_12,
            GRASS_13,
            GRASS_14,
            GRASS_15,
            GRASS_16,

            FLOWER_1,
            FLOWER_2,
            FLOWER_3,
            FLOWER_4,
            FLOWER_5,
            FLOWER_6,
            FLOWER_7,
            FLOWER_8,
            FLOWER_9,
            FLOWER_10,
            FLOWER_11,
            FLOWER_12,
            FLOWER_13,
            FLOWER_14,
            FLOWER_15,
            FLOWER_16,

            STONE_PATH
        };

    public:
        static std::map<Type, std::unique_ptr<Tile>> tiles;
        static std::vector<std::unique_ptr<Tile>> uniqueTiles;

    public:
        static void InitRegistry();
        static void RegisterTile(Type tileType, Tile* tile);
        static Tile* GetTile(Type tileType);
        static Tile* GetTileUnique(Type tileType);
    };
}
