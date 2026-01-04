#include "interspace/client/Tiles.hpp"

#include <random>

#include "interspace/game/DBHelper.hpp"
#include "SQLiteCpp/Backup.h"

namespace Interspace::Client
{
    void Tiles::Init()
    {
        SQLite::Database* db = DBHelper::db.get();

        SQLite::Statement statement(*db, R"(
            SELECT * FROM tileData
        )");

        while (statement.executeStep())
        {
            uint32_t tileId = statement.getColumn(0).getUInt();
            uint32_t tileVariant = statement.getColumn(1).getUInt();
            std::string tileName = statement.getColumn(2).getString();
            bool walkable = statement.getColumn(3).getUInt();
            std::string tileTexturePath = statement.getColumn(4).getString();
            uint32_t tileAtlasWidth = statement.getColumn(5).getUInt();
            uint32_t tileAtlasHeight = statement.getColumn(6).getUInt();
            uint32_t tileAtlasX = statement.getColumn(7).getUInt();
            uint32_t tileAtlasY = statement.getColumn(8).getUInt();

            std::unique_ptr<Tile> tile = std::make_unique<Tile>();
            tile->data.tileId = tileId;
            tile->data.variant = tileVariant;
            tile->data.tileName = tileName;
            tile->data.walkable = walkable;
            tile->texturePath = tileTexturePath;
            tile->atlasWidth = tileAtlasWidth;
            tile->atlasHeight = tileAtlasHeight;
            tile->atlasX = tileAtlasX;
            tile->atlasY = tileAtlasY;

            tiles[tileId][tileVariant] = std::move(tile);
        }
    }

    Tile* Tiles::GetRandomTileOfType(const std::string& tileName)
    {
        if (!DBHelper::TileDataExistsByName(tileName))
            return nullptr;

        std::vector<uint32_t> tileVariants = DBHelper::GetTileDataIdsByName(tileName);

        int tileIdsCount = tileVariants.size();
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> tileIdDist(0, tileIdsCount - 1);

        uint32_t tileVariant = tileIdDist(gen);
        uint32_t tileId = DBHelper::GetTileDataIdByName(tileName, tileVariant);

        if (!tiles.contains(tileId))
            return nullptr;
        if (!tiles[tileId].contains(tileVariant))
            return nullptr;

        Tile* tile = tiles[tileId][tileVariant].get();
        return tile;
    }

    Tile* Tiles::GetTileOfType(const std::string& tileName, uint32_t tileVariant)
    {
        if (!DBHelper::TileDataExistsByName(tileName))
            return nullptr;

        uint32_t tileId = DBHelper::GetTileDataIdByName(tileName, tileVariant);

        if (!tiles.contains(tileId))
            return nullptr;
        if (!tiles[tileId].contains(tileVariant))
            return nullptr;

        Tile* tile = tiles[tileId][tileVariant].get();
        return tile;
    }

    Tile* Tiles::GetTileOfType(uint32_t tileId, uint32_t tileVariant)
    {
        if (!DBHelper::TileDataExists(tileId, tileVariant))
            return nullptr;

        if (!tiles.contains(tileId))
            return nullptr;
        if (!tiles[tileId].contains(tileVariant))
            return nullptr;

        Tile* tile = tiles[tileId][tileVariant].get();

        return tile;
    }
}
