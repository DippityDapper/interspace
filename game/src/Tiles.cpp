#include "game/Tiles.hpp"

#include "dapper2d/Sprite.hpp"

#include "game/Tile.hpp"

namespace Game
{
    std::map<TileType, std::unique_ptr<Tile>> Tiles::tiles{};
    std::vector<std::unique_ptr<Tile>> Tiles::uniqueTiles{};

    void Tiles::InitRegistry()
    {
        for (int i = 0; i < ((int)GRASS_16 - (int)GRASS_1) + 1; ++i)
        {
            int x = i % 4;
            int y = i / 4;

            TileType type = (TileType)((int)GRASS_1 + i);
            Tile* tile = new Tile("assets/tilesets/grass_tileset.png", 32, 32, x, y);

            tile->type = type;
            RegisterTile(type, tile);
        }

        for (int i = 0; i < ((int)FLOWER_16 - (int)FLOWER_1) + 1; ++i)
        {
            int x = (i % 4) + 4;
            int y = i / 4;

            TileType type = (TileType)((int)FLOWER_1 + i);
            Tile* tile = new Tile("assets/tilesets/grass_tileset.png", 32, 32, x, y);

            tile->type = type;
            RegisterTile(type, tile);
        }

        TileType type = STONE_PATH;
        Tile* tile = new Tile("assets/tilesets/grass_tileset.png", 32, 32, 0, 4);

        tile->type = type;
        RegisterTile(type, tile);
    }

    void Tiles::RegisterTile(TileType tileType, Tile *tile)
    {
        if (tiles.contains(tileType))
            return;
        tiles.emplace(tileType, tile);
    }

    Tile* Tiles::GetTile(TileType tileType)
    {
        if (!tiles.contains(tileType))
            return nullptr;
        return tiles[tileType].get();
    }

    Tile* Tiles::GetTileUnique(TileType tileType)
    {
        if (!tiles.contains(tileType))
            return nullptr;

        uniqueTiles.push_back(std::make_unique<Tile>(*tiles[tileType]));
        return uniqueTiles[uniqueTiles.size()-1].get();
    }
}