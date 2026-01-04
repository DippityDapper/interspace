#include "interspace/world/TileRegistry.hpp"

#include "interspace/game/DBHelper.hpp"
#include "interspace/world/TileType.hpp"

namespace Interspace
{
    void TileRegistry::Init()
    {
        if (!DBHelper::TileDataExistsByName("grass"))
        {
            for (int variant = 0; variant < static_cast<int>(GRASS_16) - static_cast<int>(GRASS_1) + 1; ++variant)
            {
                int atlasX = variant % 4;
                int atlasY = variant / 4;
                DBHelper::InsertTileData(
                    1,
                    variant,
                    "grass",
                    true,
                    "assets/tilesets/grass_tileset.png",
                    32,
                    32,
                    atlasX,
                    atlasY
                );
            }
        }

        if (!DBHelper::TileDataExistsByName("grass_flower"))
        {
            for (int variant = 0; variant < static_cast<int>(FLOWER_16) - static_cast<int>(FLOWER_1) + 1; ++variant)
            {
                int atlasX = variant % 4;
                int atlasY = variant / 4;
                DBHelper::InsertTileData(
                    2,
                    variant,
                    "grass_flower",
                    true,
                    "assets/tilesets/grass_tileset.png",
                    32,
                    32,
                    atlasX,
                    atlasY
                );
            }
        }

        if (!DBHelper::TileDataExistsByName("grass_stone_path"))
        {
            DBHelper::InsertTileData(
                3,
                0,
                "grass_stone_path",
                true,
                "assets/tilesets/grass_tileset.png",
                32,
                32,
                0,
                4
            );
        }
    }
}
