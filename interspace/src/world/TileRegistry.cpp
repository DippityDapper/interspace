#include "interspace/world/TileRegistry.hpp"

namespace Interspace
{
    void TileRegistry::Init()
    {
        for (int i = 0; i < static_cast<int>(GRASS_16) - static_cast<int>(GRASS_1) + 1; ++i)
        {
            TileType type = (TileType)(static_cast<int>(GRASS_1) + i);
            tiles.emplace(type, std::make_unique<TileData>(type));
        }

        for (int i = 0; i < static_cast<int>(FLOWER_16) - static_cast<int>(FLOWER_1) + 1; ++i)
        {
            TileType type = (TileType)(static_cast<int>(FLOWER_1) + i);
            tiles.emplace(type, std::make_unique<TileData>(type));
        }

        TileType type = STONE_PATH;
        tiles.emplace(type, std::make_unique<TileData>(type));
    }

    TileData* TileRegistry::GetTile(TileType tileType)
    {
        if (!tiles.contains(tileType))
            return nullptr;
        return tiles[tileType].get();
    }
}
