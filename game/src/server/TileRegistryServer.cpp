#include "game/server/TileRegistryServer.hpp"

#include "game/server/TileServer.hpp"

namespace Game
{
    void TileRegistryServer::InitRegistry()
    {
        for (int i = 0; i < ((int)GRASS_16 - (int)GRASS_1) + 1; ++i)
        {
            int x = i % 4;
            int y = i / 4;

            TileType type = (TileType)((int)GRASS_1 + i);
            TileServer* tile = new TileServer(type);

            tile->type = type;
            RegisterTile(type, tile);
        }

        for (int i = 0; i < ((int)FLOWER_16 - (int)FLOWER_1) + 1; ++i)
        {
            int x = (i % 4) + 4;
            int y = i / 4;

            TileType type = (TileType)((int)FLOWER_1 + i);
            TileServer* tile = new TileServer(type);

            tile->type = type;
            RegisterTile(type, tile);
        }

        TileType type = STONE_PATH;
        TileServer* tile = new TileServer(type);

        tile->type = type;
        RegisterTile(type, tile);
    }

    void TileRegistryServer::RegisterTile(TileType tileType, TileServer *tile)
    {
        if (tiles.contains(tileType))
            return;
        tiles.emplace(tileType, tile);
    }

    TileServer* TileRegistryServer::GetTile(TileType tileType)
    {
        if (!tiles.contains(tileType))
            return nullptr;
        return tiles[tileType].get();
    }
}