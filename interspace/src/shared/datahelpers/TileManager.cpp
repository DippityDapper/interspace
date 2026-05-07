#include "interspace/shared/datahelpers/TileManager.hpp"

#include "SQLiteCpp/Database.h"
#include "interspace/shared/datahelpers/DatabaseManager.hpp"
#include "interspace/shared/world/TileType.hpp"

#include <random>

namespace Interspace
{
    void TileManager::Init()
    {
        if (!std::filesystem::exists("data"))
            std::filesystem::create_directory("data");
        if (!std::filesystem::exists("data/shared"))
            std::filesystem::create_directory("data/shared");

        CreateTables();

        SQLite::Database* db = DatabaseManager::GetSharedDatabase();
        SQLite::Statement statement(*db, R"(
            SELECT tileId, tileVariant, tileName, walkable FROM tileData
        )");

        while (statement.executeStep())
        {
            uint32_t tileId = statement.getColumn(0).getUInt();
            uint32_t tileVariant = statement.getColumn(1).getUInt();
            std::string tileName = statement.getColumn(2).getString();
            bool walkable = statement.getColumn(3).getUInt();

            std::unique_ptr<TileData> tile = std::make_unique<TileData>();
            tile->tileId = tileId;
            tile->variant = tileVariant;
            tile->tileName = tileName;
            tile->walkable = walkable;

            tiles[tileId][tileVariant] = std::move(tile);
            tileNameToId.emplace(tileName, tileId);
            if (!tileVariantsByName.contains(tileName))
                tileVariantsByName[tileName] = {};
            tileVariantsByName[tileName].push_back(tileVariant);
        }

        if (!TileDataExists("grass"))
        {
            for (int variant = 0; variant < static_cast<int>(GRASS_16) - static_cast<int>(GRASS_1) + 1; ++variant)
            {
                int atlasX = variant % 4;
                int atlasY = variant / 4;
                InsertTileData(1, variant, "grass", true, "assets/tilesets/grass_tileset.png", 32, 32, atlasX, atlasY);
                AddTileData(1, variant, "grass", true, "assets/tilesets/grass_tileset.png", 32, 32, atlasX, atlasY);
            }
        }

        if (!TileDataExists("grass_flower"))
        {
            for (int variant = 0; variant < static_cast<int>(FLOWER_16) - static_cast<int>(FLOWER_1) + 1; ++variant)
            {
                int atlasX = variant % 4 + 4;
                int atlasY = variant / 4;
                InsertTileData(2, variant, "grass_flower", true, "assets/tilesets/grass_tileset.png", 32, 32, atlasX, atlasY);
                AddTileData(2, variant, "grass_flower", true, "assets/tilesets/grass_tileset.png", 32, 32, atlasX, atlasY);
            }
        }

        if (!TileDataExists("grass_stone_path"))
        {
            InsertTileData(3, 0, "grass_stone_path", true, "assets/tilesets/grass_tileset.png", 32, 32, 0, 4);
            AddTileData(3, 0, "grass_stone_path", true, "assets/tilesets/grass_tileset.png", 32, 32, 0, 4);
        }
    }

    void TileManager::CreateTables()
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        db->exec(R"(
            CREATE TABLE IF NOT EXISTS tileData(
                tileId INTEGER NOT NULL,
                tileVariant INTEGER NOT NULL,
                tileName VARCHAR(255) NOT NULL,
                walkable INTEGER NOT NULL DEFAULT 1,
                tileTexturePath VARCHAR(255) NOT NULL,
                tileAtlasWidth INTEGER NOT NULL,
                tileAtlasHeight INTEGER NOT NULL,
                tileAtlasX INTEGER NOT NULL,
                tileAtlasY INTEGER NOT NULL,
                PRIMARY KEY(tileId, tileVariant)
            );
        )");
    }

    bool TileManager::TileDataExists(const std::string& tileName)
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        SQLite::Statement query(*db, "SELECT tileId FROM tileData WHERE tileName = ?");
        query.bind(1, tileName);
        return query.executeStep();
    }

    bool TileManager::InsertTileData(uint32_t tileId, uint32_t tileVariant, const std::string& tileName, bool walkable, const std::string& texturePath, uint32_t atlasW, uint32_t atlasH, uint32_t atlasX, uint32_t atlasY)
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        SQLite::Statement statement(*db, R"(
            INSERT OR REPLACE INTO tileData(
                tileId, tileVariant, tileName, walkable,
                tileTexturePath, tileAtlasWidth, tileAtlasHeight, tileAtlasX, tileAtlasY)
            VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)
        )");

        statement.bind(1, tileId);
        statement.bind(2, tileVariant);
        statement.bind(3, tileName);
        statement.bind(4, walkable ? 1 : 0);
        statement.bind(5, texturePath);
        statement.bind(6, atlasW);
        statement.bind(7, atlasH);
        statement.bind(8, atlasX);
        statement.bind(9, atlasY);

        return statement.exec() > 0;
    }

    bool TileManager::DeleteTileData(uint32_t tileId)
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        SQLite::Statement statement(*db, R"(
            DELETE FROM tileData WHERE tileId = ?
        )");
        statement.bind(1, tileId);
        return statement.exec() > 0;
    }

    void TileManager::AddTileData(uint32_t tileId, uint32_t tileVariant, const std::string& tileName, bool walkable, const std::string& texturePath, uint32_t atlasW, uint32_t atlasH, uint32_t atlasX, uint32_t atlasY)
    {
        std::unique_ptr<TileData> tile = std::make_unique<TileData>();
        tile->tileId = tileId;
        tile->variant = tileVariant;
        tile->tileName = tileName;
        tile->walkable = walkable;

        Engine::Vec2<float> tempPos{};
        tile->sprite = std::make_unique<Engine::Sprite>(tempPos, texturePath, atlasW, atlasH, atlasX, atlasY);

        tiles[tileId][tileVariant] = std::move(tile);
        tileNameToId.emplace(tileName, tileId);
        if (!tileVariantsByName.contains(tileName))
            tileVariantsByName[tileName] = {};
        tileVariantsByName[tileName].push_back(tileVariant);
    }

    void TileManager::RemoveTileData(uint32_t tileId, const std::string& tileName)
    {
        if (tiles.contains(tileId))
            tiles.erase(tileId);
        if (tileNameToId.contains(tileName))
            tileNameToId.erase(tileName);
        if (!tileVariantsByName.contains(tileName))
            tileVariantsByName.erase(tileName);
    }

    bool TileManager::TileExists(uint32_t tileId)
    {
        if (!tiles.contains(tileId))
            return false;
        return true;
    }

    bool TileManager::TileVariantExists(uint32_t tileId, uint32_t tileVariant)
    {
        if (!tiles.contains(tileId))
            return false;
        if (!tiles[tileId].contains(tileVariant))
            return false;
        return true;
    }

    uint32_t TileManager::GetTileId(const std::string& tileName)
    {
        if (!tileNameToId.contains(tileName))
            return 0;
        return tileNameToId[tileName];
    }

    TileData* TileManager::GetTile(uint32_t tileId, uint32_t tileVariant)
    {
        if (!tiles.contains(tileId))
            return nullptr;
        if (!tiles[tileId].contains(tileVariant))
            return nullptr;
        return tiles[tileId][tileVariant].get();
    }

    std::vector<uint32_t> TileManager::GetTileVariants(const std::string& tileName)
    {
        if (!tileVariantsByName.contains(tileName))
            return {};
        return tileVariantsByName[tileName];
    }

    TileData* TileManager::GetTileOfType(const std::string& tileName, uint32_t tileVariant)
    {
        uint32_t tileId = GetTileId(tileName);

        if (!TileExists(tileId))
            return nullptr;
        if (!TileVariantExists(tileId, tileVariant))
            return nullptr;

        TileData* tile = GetTile(tileId, tileVariant);
        return tile;
    }

    TileData* TileManager::GetTileOfType(uint32_t tileId, uint32_t tileVariant)
    {
        if (!TileExists(tileId))
            return nullptr;
        if (!TileVariantExists(tileId, tileVariant))
            return nullptr;

        TileData* tile = GetTile(tileId, tileVariant);
        return tile;
    }

    TileData* TileManager::GetRandomTileBySeed(const std::string& tileName, uint32_t worldSeed, uint16_t chunkX, uint16_t chunkY)
    {
        std::vector<uint32_t> tileVariants = GetTileVariants(tileName);
        uint32_t tileVariantCount = tileVariants.size();

        std::uniform_int_distribution<uint32_t> tileVariantDist(0, tileVariantCount - 1);

        uint32_t chunkSeed = worldSeed ^ (chunkX * 73856093) ^ (chunkY * 19349663);
        std::mt19937 chunkGenerator(chunkSeed);
        uint32_t tileVariant = tileVariantDist(chunkGenerator);
        uint32_t tileId = GetTileId(tileName);

        if (!TileExists(tileId))
            return nullptr;
        if (!TileVariantExists(tileId, tileVariant))
            return nullptr;

        TileData* tile = GetTile(tileId, tileVariant);
        return tile;
    }
}