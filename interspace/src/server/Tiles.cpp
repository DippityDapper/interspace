#include "interspace/server/Tiles.hpp"

#include <random>

#include "SQLiteCpp/Database.h"
#include "interspace/game/DBHelper.hpp"

namespace Interspace::Server
{
    void Tiles::Init()
    {
        SQLite::Database* commonDb = DBHelper::commonDb.get();

        SQLite::Statement statement(*commonDb, R"(
            SELECT tileId, tileVariant, tileName, walkable FROM tileData
        )");

        while (statement.executeStep())
        {
            uint32_t tileId = statement.getColumn(0).getUInt();
            uint32_t tileVariant = statement.getColumn(1).getUInt();
            std::string tileName = statement.getColumn(2).getString();
            bool walkable = statement.getColumn(3).getUInt();

            std::unique_ptr<Tile> tile = std::make_unique<Tile>();
            tile->data.tileId = tileId;
            tile->data.variant = tileVariant;
            tile->data.tileName = tileName;
            tile->data.walkable = walkable;

            tiles[tileId][tileVariant] = std::move(tile);
        }
    }

    Tile* Tiles::GetRandomTileBySeed(const std::string& tileName, std::mt19937& tileGen)
    {
        if (!tileNameToId.contains(tileName))
        {
            if (!DBHelper::TileDataExists(tileName))
            {
                return nullptr;
            }
            tileNameToId.emplace(tileName, DBHelper::GetTileDataId(tileName));
        }

        if (!tileVariantsByName.contains(tileName))
        {
            tileVariantsByName.emplace(tileName, DBHelper::GetTileDataVariants(tileName));
        }

        std::vector<uint32_t> tileVariants = tileVariantsByName[tileName];
        int tileVariantCount = tileVariants.size();

        std::uniform_int_distribution<> tileVariantDist(0, tileVariantCount - 1);

        uint32_t tileVariant = tileVariantDist(tileGen);
        uint32_t tileId = tileNameToId[tileName];

        if (!tiles.contains(tileId))
            return nullptr;
        if (!tiles[tileId].contains(tileVariant))
            return nullptr;

        Tile* tile = tiles[tileId][tileVariant].get();
        return tile;
    }

    Tile* Tiles::GetTileOfType(const std::string& tileName, uint32_t tileVariant)
    {
        if (!tileNameToId.contains(tileName))
        {
            if (!DBHelper::TileDataExists(tileName))
            {
                return nullptr;
            }
            tileNameToId.emplace(tileName, DBHelper::GetTileDataId(tileName));
        }

        uint32_t tileId = tileNameToId[tileName];

        if (!tiles.contains(tileId))
            return nullptr;
        if (!tiles[tileId].contains(tileVariant))
            return nullptr;

        Tile* tile = tiles[tileId][tileVariant].get();
        return tile;
    }

    Tile* Tiles::GetTileOfType(uint32_t tileId, uint32_t tileVariant)
    {
        if (!tiles.contains(tileId))
            return nullptr;
        if (!tiles[tileId].contains(tileVariant))
            return nullptr;

        Tile* tile = tiles[tileId][tileVariant].get();

        return tile;
    }
}
