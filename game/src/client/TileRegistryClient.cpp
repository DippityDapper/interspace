#include "game/client/TileRegistryClient.hpp"

namespace Game
{
    void TileRegistryClient::InitRegistry()
    {
        for (int i = 0; i < ((int)GRASS_16 - (int)GRASS_1) + 1; ++i)
        {
            int x = i % 4;
            int y = i / 4;

            TileType type = (TileType)((int)GRASS_1 + i);
            TileClient* tile = new TileClient(type, "assets/tilesets/grass_tileset.png", 32, 32, x, y);

            tile->type = type;
            RegisterTile(type, tile);
        }

        for (int i = 0; i < ((int)FLOWER_16 - (int)FLOWER_1) + 1; ++i)
        {
            int x = (i % 4) + 4;
            int y = i / 4;

            TileType type = (TileType)((int)FLOWER_1 + i);
            TileClient* tile = new TileClient(type, "assets/tilesets/grass_tileset.png", 32, 32, x, y);

            tile->type = type;
            RegisterTile(type, tile);
        }

        TileType type = STONE_PATH;
        TileClient* tile = new TileClient(type, "assets/tilesets/grass_tileset.png", 32, 32, 0, 4);

        tile->type = type;
        RegisterTile(type, tile);
    }

    void TileRegistryClient::RegisterTile(TileType tileType, TileClient *tile)
    {
        if (tiles.contains(tileType))
            return;
        tiles.emplace(tileType, tile);
    }

    TileClient* TileRegistryClient::GetTile(TileType tileType)
    {
        if (!tiles.contains(tileType))
            return nullptr;
        return tiles[tileType].get();
    }

    std::map<TileType, TileClient*> TileRegistryClient::GetTiles()
    {
        std::map<TileType, TileClient*> _tiles;
        for (const auto& tile : tiles)
        {
            _tiles[tile.first] = tile.second.get();
        }
        return _tiles;
    }
}
