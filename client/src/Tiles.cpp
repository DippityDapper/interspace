#include "client/Tiles.hpp"

#include "dapper2d/Sprite.hpp"

#include "client/Tile.hpp"

namespace Game
{
    std::map<Tiles::Type, Tile*> Tiles::tiles{};
    std::vector<Tile*> Tiles::uniqueTiles{};

    void Tiles::InitRegistry()
    {
        for (int i = 0; i < ((int)Tiles::GRASS_16 - (int)Tiles::GRASS_1) + 1; ++i)
        {
            int x = i % 4;
            int y = i / 4;

            RegisterTile((Type)((int)Tiles::GRASS_1 + i), new Tile("tilesets/grass_tileset.png", 32, 32, x, y));
        }

        for (int i = 0; i < ((int)Tiles::FLOWER_16 - (int)Tiles::FLOWER_1) + 1; ++i)
        {
            int x = (i % 4) + 4;
            int y = i / 4;

            RegisterTile((Type)((int)Tiles::FLOWER_1 + i), new Tile("tilesets/grass_tileset.png", 32, 32, x, y));
        }


        RegisterTile(Tiles::STONE_PATH, new Tile("tilesets/grass_tileset.png", 32, 32, 0, 4));
    }

    void Tiles::RegisterTile(Tiles::Type tileType, Tile *tile)
    {
        if (tiles.contains(tileType))
            return;
        tiles.emplace(tileType, tile);
    }

    Tile* Tiles::GetTile(Type tileType)
    {
        if (!tiles.contains(tileType))
            return nullptr;
        return tiles[tileType];
    }

    Tile* Tiles::GetTileUnique(Type tileType)
    {
        if (!tiles.contains(tileType))
            return nullptr;

        Tile* tile = new Tile(*tiles[tileType]);
        uniqueTiles.push_back(tile);
        return tile;
    }

    Tiles::~Tiles()
    {
        for (auto& kvp : tiles)
        {
            delete kvp.second;
        }
        for (Tile* tile : uniqueTiles)
        {
            delete tile;
        }
    }
}