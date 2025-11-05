#include "client/Tiles.hpp"

#include "dapper2d/Sprite.hpp"

#include "client/Tile.hpp"

namespace Game
{
    std::map<Tiles::Type, std::unique_ptr<Tile>> Tiles::tiles{};
    std::vector<std::unique_ptr<Tile>> Tiles::uniqueTiles{};

    void Tiles::InitRegistry()
    {
        for (int i = 0; i < ((int)Tiles::GRASS_16 - (int)Tiles::GRASS_1) + 1; ++i)
        {
            int x = i % 4;
            int y = i / 4;

            Type type = (Type)((int)Tiles::GRASS_1 + i);
            Tile* tile = new Tile("assets/tilesets/grass_tileset.png", 32, 32, x, y);
            tile->type = type;
            RegisterTile(type, tile);
        }

        for (int i = 0; i < ((int)Tiles::FLOWER_16 - (int)Tiles::FLOWER_1) + 1; ++i)
        {
            int x = (i % 4) + 4;
            int y = i / 4;

            Type type = (Type)((int)Tiles::FLOWER_1 + i);
            Tile* tile = new Tile("assets/tilesets/grass_tileset.png", 32, 32, x, y);
            tile->type = type;
            RegisterTile(type, tile);
        }

        RegisterTile(Tiles::STONE_PATH, new Tile("assets/tilesets/grass_tileset.png", 32, 32, 0, 4));
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
        return tiles[tileType].get();
    }

    Tile* Tiles::GetTileUnique(Type tileType)
    {
        if (!tiles.contains(tileType))
            return nullptr;

        uniqueTiles.push_back(std::make_unique<Tile>(*tiles[tileType]));
        return uniqueTiles[uniqueTiles.size()-1].get();
    }
}