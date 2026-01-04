#include "interspace/game/DBHelper.hpp"
#include "SQLiteCpp/Statement.h"

namespace Interspace
{
    void DBHelper::InitDatabase()
    {
        db = std::make_unique<SQLite::Database>("data/worlds.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        db->exec("PRAGMA foreign_keys = ON;");

        CreateWorldTable();
        CreateChunkTable();
        CreateTileDataTable();
        CreateTileTable();
        CreatePlayerTable();
        CreateFactionTable();
        CreateColonistTable();
        CreateFactionMemberTable();
        CreateIndices();
    }

    // ============================================================
    // TABLE CREATION
    // ============================================================

    void DBHelper::CreateWorldTable()
    {
        db->exec(R"(
            CREATE TABLE IF NOT EXISTS world(
                worldId VARCHAR(255) PRIMARY KEY,
                worldSeed INTEGER NOT NULL,
                worldSizeX INTEGER NOT NULL,
                worldSizeY INTEGER NOT NULL,
                createdAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                lastPlayed INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                gameVersion VARCHAR(50)
            );
        )");
    }

    void DBHelper::CreateChunkTable()
    {
        db->exec(R"(
            CREATE TABLE IF NOT EXISTS chunk(
                worldId VARCHAR(255) NOT NULL,
                chunkX INTEGER NOT NULL,
                chunkY INTEGER NOT NULL,
                PRIMARY KEY (worldId, chunkX, chunkY),
                FOREIGN KEY (worldId) REFERENCES world(worldId) ON DELETE CASCADE
            );
        )");
    }

    void DBHelper::CreateTileTable()
    {
        db->exec(R"(
            CREATE TABLE IF NOT EXISTS tile(
                worldId VARCHAR(255) NOT NULL,
                chunkX INTEGER NOT NULL,
                chunkY INTEGER NOT NULL,
                tileX INTEGER NOT NULL,
                tileY INTEGER NOT NULL,
                tileType INTEGER NOT NULL,
                PRIMARY KEY (worldId, chunkX, chunkY, tileX, tileY),
                FOREIGN KEY (worldId, chunkX, chunkY) REFERENCES chunk(worldId, chunkX, chunkY) ON DELETE CASCADE,
                FOREIGN KEY (tileType) REFERENCES tileData(tileId) ON DELETE RESTRICT
            );
        )");
    }

    void DBHelper::CreatePlayerTable()
    {
        db->exec(R"(
            CREATE TABLE IF NOT EXISTS player(
                playerId INTEGER PRIMARY KEY,
                playerName VARCHAR(255) NOT NULL UNIQUE,
                createdAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now'))
            );
        )");
    }

    void DBHelper::CreateFactionTable()
    {
        db->exec(R"(
            CREATE TABLE IF NOT EXISTS faction(
                factionId INTEGER NOT NULL,
                worldId VARCHAR(255) NOT NULL,
                factionName VARCHAR(255) NOT NULL,
                factionOwner INTEGER,
                createdAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                PRIMARY KEY (factionId, worldId),
                FOREIGN KEY (worldId) REFERENCES world(worldId) ON DELETE CASCADE,
                FOREIGN KEY (factionOwner) REFERENCES player(playerId) ON DELETE CASCADE
            );
        )");
    }

    void DBHelper::CreateColonistTable()
    {
        db->exec(R"(
            CREATE TABLE IF NOT EXISTS colonist(
                colonistId INTEGER PRIMARY KEY,
                worldId VARCHAR(255) NOT NULL,
                factionId INTEGER NOT NULL,
                colonistName VARCHAR(255) NOT NULL,
                colonistX REAL NOT NULL,
                colonistY REAL NOT NULL,
                createdAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                FOREIGN KEY (factionId, worldId) REFERENCES faction(factionId, worldId) ON DELETE CASCADE
            );
        )");
    }

    void DBHelper::CreateFactionMemberTable()
    {
        db->exec(R"(
            CREATE TABLE IF NOT EXISTS factionMember(
                factionId INTEGER NOT NULL,
                worldId VARCHAR(255) NOT NULL,
                playerId INTEGER NOT NULL,
                joinedAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                PRIMARY KEY (factionId, worldId, playerId),
                FOREIGN KEY (factionId, worldId) REFERENCES faction(factionId, worldId) ON DELETE CASCADE,
                FOREIGN KEY (playerId) REFERENCES player(playerId) ON DELETE CASCADE
            );
        )");
    }

    void DBHelper::CreateTileDataTable()
    {
        db->exec("DROP TABLE IF EXISTS tileData");

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

    void DBHelper::CreateIndices()
    {
        db->exec(R"(
            CREATE INDEX IF NOT EXISTS idx_tile_chunk_lookup
            ON tile(worldId, chunkX, chunkY);
        )");

        db->exec(R"(
            CREATE INDEX IF NOT EXISTS idx_colonist_world_position
            ON colonist(worldId, colonistX, colonistY);
        )");

        db->exec(R"(
            CREATE INDEX IF NOT EXISTS idx_colonist_faction
            ON colonist(factionId);
        )");

        db->exec(R"(
            CREATE INDEX IF NOT EXISTS idx_faction_world
            ON faction(worldId);
        )");

        db->exec(R"(
            CREATE INDEX IF NOT EXISTS idx_faction_member_player
            ON factionMember(playerId);
        )");
    }

    // ============================================================
    // EXISTS CHECKS
    // ============================================================

    bool DBHelper::WorldExists(const std::string& worldId)
    {
        SQLite::Statement query(*db, "SELECT worldId FROM world WHERE worldId = ?");
        query.bind(1, worldId);
        return query.executeStep();
    }

    bool DBHelper::ChunkExists(const std::string& worldId, int32_t chunkX, int32_t chunkY)
    {
        SQLite::Statement query(*db, "SELECT 1 FROM chunk WHERE worldId = ? AND chunkX = ? AND chunkY = ?");
        query.bind(1, worldId);
        query.bind(2, chunkX);
        query.bind(3, chunkY);
        return query.executeStep();
    }

    bool DBHelper::TileExists(const std::string& worldId, int32_t chunkX, int32_t chunkY, int32_t tileX, int32_t tileY)
    {
        SQLite::Statement query(*db, "SELECT 1 FROM tile WHERE worldId = ? AND chunkX = ? AND chunkY = ? AND tileX = ? AND tileY = ?");
        query.bind(1, worldId);
        query.bind(2, chunkX);
        query.bind(3, chunkY);
        query.bind(4, tileX);
        query.bind(5, tileY);
        return query.executeStep();
    }

    bool DBHelper::PlayerExists(uint32_t playerId)
    {
        SQLite::Statement query(*db, "SELECT playerId FROM player WHERE playerId = ?");
        query.bind(1, static_cast<int>(playerId));
        return query.executeStep();
    }

    bool DBHelper::PlayerExistsByName(const std::string& playerName)
    {
        SQLite::Statement query(*db, "SELECT playerId FROM player WHERE playerName = ?");
        query.bind(1, playerName);
        return query.executeStep();
    }

    bool DBHelper::FactionExists(uint16_t factionId, const std::string& worldId)
    {
        SQLite::Statement query(*db, "SELECT factionId FROM faction WHERE factionId = ? AND worldId = ?");
        query.bind(1, factionId);
        query.bind(2, worldId);
        return query.executeStep();
    }

    bool DBHelper::FactionExistsByName(const std::string& worldId, const std::string& factionName)
    {
        SQLite::Statement query(*db, "SELECT factionId FROM faction WHERE factionName = ? AND worldId = ?");
        query.bind(1, factionName);
        query.bind(2, worldId);
        return query.executeStep();
    }

    bool DBHelper::ColonistExists(const std::string& worldId, uint16_t colonistId)
    {
        SQLite::Statement query(*db, "SELECT colonistId FROM colonist WHERE colonistId = ? AND worldId = ?");
        query.bind(1, colonistId);
        query.bind(2, worldId);
        return query.executeStep();
    }

    bool DBHelper::ColonistExistsByName(const std::string& worldId, const std::string& colonistName)
    {
        SQLite::Statement query(*db, "SELECT colonistId FROM colonist WHERE colonistName = ? AND worldId = ?");
        query.bind(1, colonistName);
        query.bind(2, worldId);
        return query.executeStep();
    }

    bool DBHelper::TileDataExistsByName(const std::string& tileName)
    {
        SQLite::Statement query(*db, "SELECT tileId FROM tileData WHERE tileName = ?");
        query.bind(1, tileName);
        return query.executeStep();
    }

    bool DBHelper::TileDataExists(uint32_t tileId, uint32_t tileVariant)
    {
        SQLite::Statement query(*db, "SELECT tileId FROM tileData WHERE tileId = ? AND tileVariant = ?");
        query.bind(1, tileId);
        query.bind(2, tileVariant);
        return query.executeStep();
    }

    // ============================================================
    // GET METHODS
    // ============================================================

    uint16_t DBHelper::GetFactionIdByName(const std::string& worldId, const std::string& factionName)
    {
        SQLite::Statement query(*db, "SELECT factionId FROM faction WHERE factionName = ? AND worldId = ?");
        query.bind(1, factionName);
        query.bind(2, worldId);

        if (query.executeStep())
        {
            return static_cast<uint16_t>(query.getColumn(0).getInt());
        }
        return 0;
    }

    std::vector<uint32_t> DBHelper::GetFactionMemberIds(const std::string& worldId, uint16_t factionId)
    {
        std::vector<uint32_t> memberIds;
        SQLite::Statement query(*db, "SELECT playerId FROM factionMember WHERE factionId = ? AND worldId = ?");
        query.bind(1, factionId);
        query.bind(2, worldId);

        while (query.executeStep())
        {
            memberIds.push_back(static_cast<uint32_t>(query.getColumn(0).getInt()));
        }
        return memberIds;
    }

    uint16_t DBHelper::GetPlayerFactionId(const std::string& worldId, uint32_t playerId)
    {
        SQLite::Statement query(*db, "SELECT factionId FROM factionMember WHERE worldId = ? AND playerId = ? LIMIT 1");
        query.bind(1, worldId);
        query.bind(2, static_cast<int>(playerId));

        if (query.executeStep())
        {
            return static_cast<uint16_t>(query.getColumn(0).getInt());
        }
        return 0;
    }

    std::vector<uint16_t> DBHelper::GetPlayerFactionIds(const std::string& worldId, uint32_t playerId)
    {
        std::vector<uint16_t> factionIds{};
        SQLite::Statement query(*db, "SELECT factionId FROM factionMember WHERE worldId = ? AND playerId = ?");
        query.bind(1, worldId);
        query.bind(2, static_cast<int>(playerId));

        while (query.executeStep())
        {
            factionIds.push_back(static_cast<uint16_t>(query.getColumn(0).getInt()));
        }
        return factionIds;
    }

    uint16_t DBHelper::GetColonistFactionId(const std::string& worldId, uint16_t colonistId)
    {
        SQLite::Statement query(*db, "SELECT factionId FROM colonist WHERE colonistId = ? AND worldId = ?");
        query.bind(1, colonistId);
        query.bind(2, worldId);

        if (query.executeStep())
        {
            return static_cast<uint16_t>(query.getColumn(0).getInt());
        }
        return 0;
    }

    uint16_t DBHelper::GetColonistByName(const std::string& worldId, const std::string& colonistName)
    {
        SQLite::Statement query(*db, "SELECT colonistId FROM colonist WHERE colonistName = ? AND worldId = ?");
        query.bind(1, colonistName);
        query.bind(2, worldId);

        if (query.executeStep())
        {
            return static_cast<uint16_t>(query.getColumn(0).getInt());
        }
        return 0;
    }

    Engine::Vec2<float> DBHelper::GetColonistPosition(const std::string& worldId, uint16_t colonistId)
    {
        SQLite::Statement query(*db, "SELECT colonistX, colonistY FROM colonist WHERE colonistId = ? AND worldId = ?");
        query.bind(1, colonistId);
        query.bind(2, worldId);

        if (query.executeStep())
        {
            float posX = static_cast<float>(query.getColumn(0).getDouble());
            float posY = static_cast<float>(query.getColumn(1).getDouble());
            return {posX, posY};
        }
        return {0, 0};
    }

    uint32_t DBHelper::GetPlayerIdByName(const std::string& playerName)
    {
        SQLite::Statement query(*db, "SELECT playerId FROM player WHERE playerName = ?");
        query.bind(1, playerName);

        if (query.executeStep())
        {
            return static_cast<uint32_t>(query.getColumn(0).getInt());
        }
        return 0;
    }

    uint32_t DBHelper::GetTileDataIdByName(const std::string& tileName, uint32_t tileVariant)
    {
        SQLite::Statement query(*db, "SELECT tileId FROM tileData WHERE tileName = ? AND tileVariant = ?");
        query.bind(1, tileName);
        query.bind(2, tileVariant);

        if (query.executeStep())
        {
            return static_cast<uint32_t>(query.getColumn(0).getInt());
        }
        return 0;
    }

    std::vector<uint32_t> DBHelper::GetTileDataIdsByName(const std::string& tileName)
    {
        std::vector<uint32_t> tileIds{};
        SQLite::Statement query(*db, "SELECT tileId FROM tileData WHERE tileName = ?");
        query.bind(1, tileName);

        while (query.executeStep())
        {
            tileIds.push_back(static_cast<uint32_t>(query.getColumn(0).getInt()));
        }
        return tileIds;
    }

    // ============================================================
    // INSERT METHODS
    // ============================================================

    bool DBHelper::InsertWorld(const std::string& worldId, uint32_t worldSeed, uint16_t worldSizeX, uint16_t worldSizeY, const std::string& gameVersion)
    {
        SQLite::Statement statement(*db, R"(
            INSERT INTO world(worldId, worldSeed, worldSizeX, worldSizeY, gameVersion)
            VALUES(?, ?, ?, ?, ?)
        )");

        statement.bind(1, worldId);
        statement.bind(2, static_cast<int>(worldSeed));
        statement.bind(3, worldSizeX);
        statement.bind(4, worldSizeY);
        statement.bind(5, gameVersion);

        return statement.exec() > 0;
    }

    bool DBHelper::InsertChunk(const std::string& worldId, int32_t chunkX, int32_t chunkY)
    {
        SQLite::Statement statement(*db, R"(
            INSERT INTO chunk(worldId, chunkX, chunkY)
            VALUES(?, ?, ?)
        )");

        statement.bind(1, worldId);
        statement.bind(2, chunkX);
        statement.bind(3, chunkY);

        return statement.exec() > 0;
    }

    bool DBHelper::InsertTile(const std::string& worldId, int32_t chunkX, int32_t chunkY, int32_t tileX, int32_t tileY, int32_t tileType)
    {
        SQLite::Statement statement(*db, R"(
            INSERT INTO tile(worldId, chunkX, chunkY, tileX, tileY, tileType)
            VALUES(?, ?, ?, ?, ?, ?)
        )");

        statement.bind(1, worldId);
        statement.bind(2, chunkX);
        statement.bind(3, chunkY);
        statement.bind(4, tileX);
        statement.bind(5, tileY);
        statement.bind(6, tileType);

        return statement.exec() > 0;
    }

    bool DBHelper::InsertPlayer(uint32_t playerId, const std::string& playerName)
    {
        SQLite::Statement statement(*db, R"(
            INSERT INTO player(playerId, playerName)
            VALUES(?, ?)
        )");

        statement.bind(1, static_cast<int>(playerId));
        statement.bind(2, playerName);

        return statement.exec() > 0;
    }

    bool DBHelper::InsertFaction(uint16_t factionId, const std::string& worldId, const std::string& factionName, uint32_t factionOwner)
    {
        if (factionOwner == 0)
        {
            SQLite::Statement statement(*db, R"(
                INSERT INTO faction(factionId, worldId, factionName)
                VALUES(?, ?, ?)
            )");

            statement.bind(1, factionId);
            statement.bind(2, worldId);
            statement.bind(3, factionName);

            return statement.exec() > 0;
        }
        else
        {
            SQLite::Statement statement(*db, R"(
                INSERT INTO faction(factionId, worldId, factionName, factionOwner)
                VALUES(?, ?, ?, ?)
            )");

            statement.bind(1, factionId);
            statement.bind(2, worldId);
            statement.bind(3, factionName);
            statement.bind(4, static_cast<int>(factionOwner));

            return statement.exec() > 0;
        }
    }

    bool DBHelper::InsertColonist(uint16_t colonistId, const std::string& worldId, uint16_t factionId, const std::string& colonistName, float x, float y)
    {
        SQLite::Statement statement(*db, R"(
            INSERT INTO colonist(colonistId, worldId, factionId, colonistName, colonistX, colonistY)
            VALUES(?, ?, ?, ?, ?, ?)
        )");

        statement.bind(1, colonistId);
        statement.bind(2, worldId);
        statement.bind(3, factionId);
        statement.bind(4, colonistName);
        statement.bind(5, x);
        statement.bind(6, y);

        return statement.exec() > 0;
    }

    bool DBHelper::InsertTileData(uint32_t tileId, uint32_t tileVariant, const std::string& tileName, bool walkable, const std::string& texturePath, uint32_t atlasW, uint32_t atlasH, uint32_t atlasX, uint32_t atlasY)
    {
        SQLite::Statement statement(*db, R"(
            INSERT OR REPLACE INTO tileData(
                tileId, tileVariant, tileName, walkable,
                tileTexturePath, tileAtlasWidth, tileAtlasHeight, tileAtlasX, tileAtlasY)
            VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)
        )");

        statement.bind(1, static_cast<int>(tileId));
        statement.bind(2, static_cast<int>(tileVariant));
        statement.bind(3, tileName);
        statement.bind(4, walkable ? 1 : 0);
        statement.bind(5, texturePath);
        statement.bind(6, static_cast<int>(atlasW));
        statement.bind(7, static_cast<int>(atlasH));
        statement.bind(8, static_cast<int>(atlasX));
        statement.bind(9, static_cast<int>(atlasY));

        return statement.exec() > 0;
    }

    // ============================================================
    // UPDATE METHODS
    // ============================================================

    bool DBHelper::UpdateWorld(const std::string& worldId, int32_t worldSeed, int32_t worldSizeX, int32_t worldSizeY)
    {
        SQLite::Statement statement(*db, R"(
            UPDATE world SET worldSeed = ?, worldSizeX = ?, worldSizeY = ?
            WHERE worldId = ?
        )");

        statement.bind(1, worldSeed);
        statement.bind(2, worldSizeX);
        statement.bind(3, worldSizeY);
        statement.bind(4, worldId);

        return statement.exec() > 0;
    }

    bool DBHelper::UpdateWorldLastPlayed(const std::string& worldId)
    {
        SQLite::Statement statement(*db, R"(
            UPDATE world SET lastPlayed = strftime('%s', 'now')
            WHERE worldId = ?
        )");

        statement.bind(1, worldId);

        return statement.exec() > 0;
    }

    bool DBHelper::UpdateTile(const std::string& worldId, int32_t chunkX, int32_t chunkY, int32_t tileX, int32_t tileY, int32_t tileType)
    {
        SQLite::Statement statement(*db, R"(
            UPDATE tile SET tileType = ?
            WHERE worldId = ? AND chunkX = ? AND chunkY = ? AND tileX = ? AND tileY = ?
        )");

        statement.bind(1, tileType);
        statement.bind(2, worldId);
        statement.bind(3, chunkX);
        statement.bind(4, chunkY);
        statement.bind(5, tileX);
        statement.bind(6, tileY);

        return statement.exec() > 0;
    }

    bool DBHelper::UpdatePlayerName(uint32_t playerId, const std::string& playerName)
    {
        SQLite::Statement statement(*db, R"(
            UPDATE player SET playerName = ?
            WHERE playerId = ?
        )");

        statement.bind(1, playerName);
        statement.bind(2, static_cast<int>(playerId));

        return statement.exec() > 0;
    }

    bool DBHelper::UpdateFactionName(uint16_t factionId, const std::string& factionName)
    {
        SQLite::Statement statement(*db, R"(
            UPDATE faction SET factionName = ?
            WHERE factionId = ?
        )");

        statement.bind(1, factionName);
        statement.bind(2, factionId);

        return statement.exec() > 0;
    }

    bool DBHelper::UpdateFactionOwner(uint16_t factionId, uint32_t factionOwner)
    {
        SQLite::Statement statement(*db, R"(
            UPDATE faction SET factionOwner = ?
            WHERE factionId = ?
        )");

        statement.bind(1, static_cast<int>(factionOwner));
        statement.bind(2, factionId);

        return statement.exec() > 0;
    }

    bool DBHelper::UpdateColonistPosition(const std::string& worldId, uint16_t colonistId, float x, float y)
    {
        SQLite::Statement statement(*db, R"(
            UPDATE colonist SET colonistX = ?, colonistY = ?
            WHERE colonistId = ? AND worldId = ?
        )");

        statement.bind(1, x);
        statement.bind(2, y);
        statement.bind(3, colonistId);
        statement.bind(4, worldId);

        return statement.exec() > 0;
    }

    bool DBHelper::UpdateColonistFaction(const std::string& worldId, uint16_t colonistId, uint16_t factionId)
    {
        SQLite::Statement statement(*db, R"(
            UPDATE colonist SET factionId = ?
            WHERE colonistId = ? AND worldId = ?
        )");

        statement.bind(1, factionId);
        statement.bind(2, colonistId);
        statement.bind(3, worldId);

        return statement.exec() > 0;
    }

    bool DBHelper::UpdateColonistName(const std::string& worldId, uint16_t colonistId, const std::string& colonistName)
    {
        SQLite::Statement statement(*db, R"(
            UPDATE colonist SET colonistName = ?
            WHERE colonistId = ? AND worldId = ?
        )");

        statement.bind(1, colonistName);
        statement.bind(2, colonistId);
        statement.bind(3, worldId);

        return statement.exec() > 0;
    }

    // ============================================================
    // RELATIONSHIP METHODS
    // ============================================================

    bool DBHelper::IsPlayerInFaction(const std::string& worldId, uint16_t factionId, uint32_t playerId)
    {
        SQLite::Statement query(*db, R"(
            SELECT 1 FROM factionMember
            WHERE worldId = ? AND factionId = ? AND playerId = ?
        )");

        query.bind(1, worldId);
        query.bind(2, factionId);
        query.bind(3, static_cast<int>(playerId));

        return query.executeStep();
    }

    bool DBHelper::IsPlayerInAnyFaction(const std::string& worldId, uint32_t playerId)
    {
        SQLite::Statement query(*db, R"(
            SELECT factionId FROM factionMember
            WHERE worldId = ? AND playerId = ?
        )");

        query.bind(1, worldId);
        query.bind(2, static_cast<int>(playerId));

        return query.executeStep();
    }

    bool DBHelper::AddPlayerToFaction(const std::string& worldId, uint16_t factionId, uint32_t playerId)
    {
        if (IsPlayerInFaction(worldId, factionId, playerId))
        {
            return true;
        }

        SQLite::Statement statement(*db, R"(
            INSERT INTO factionMember(worldId, factionId, playerId)
            VALUES(?, ?, ?)
        )");

        statement.bind(1, worldId);
        statement.bind(2, factionId);
        statement.bind(3, static_cast<int>(playerId));

        return statement.exec() > 0;
    }

    bool DBHelper::RemovePlayerFromFaction(const std::string& worldId, uint16_t factionId, uint32_t playerId)
    {
        SQLite::Statement statement(*db, R"(
            DELETE FROM factionMember
            WHERE worldId = ? AND factionId = ? AND playerId = ?
        )");

        statement.bind(1, worldId);
        statement.bind(2, factionId);
        statement.bind(3, static_cast<int>(playerId));

        return statement.exec() > 0;
    }

    // ============================================================
    // DELETE METHODS
    // ============================================================

    bool DBHelper::DeleteWorld(const std::string& worldId)
    {
        SQLite::Statement statement(*db, "DELETE FROM world WHERE worldId = ?");
        statement.bind(1, worldId);
        return statement.exec() > 0;
    }

    bool DBHelper::DeleteChunk(const std::string& worldId, int32_t chunkX, int32_t chunkY)
    {
        SQLite::Statement statement(*db, R"(
            DELETE FROM chunk
            WHERE worldId = ? AND chunkX = ? AND chunkY = ?
        )");

        statement.bind(1, worldId);
        statement.bind(2, chunkX);
        statement.bind(3, chunkY);

        return statement.exec() > 0;
    }

    bool DBHelper::DeleteTile(const std::string& worldId, int32_t chunkX, int32_t chunkY, int32_t tileX, int32_t tileY)
    {
        SQLite::Statement statement(*db, R"(
            DELETE FROM tile
            WHERE worldId = ? AND chunkX = ? AND chunkY = ? AND tileX = ? AND tileY = ?
        )");

        statement.bind(1, worldId);
        statement.bind(2, chunkX);
        statement.bind(3, chunkY);
        statement.bind(4, tileX);
        statement.bind(5, tileY);

        return statement.exec() > 0;
    }

    bool DBHelper::DeletePlayer(uint32_t playerId)
    {
        SQLite::Statement statement(*db, "DELETE FROM player WHERE playerId = ?");
        statement.bind(1, static_cast<int>(playerId));
        return statement.exec() > 0;
    }

    bool DBHelper::DeleteFaction(uint16_t factionId)
    {
        SQLite::Statement statement(*db, "DELETE FROM faction WHERE factionId = ?");
        statement.bind(1, factionId);
        return statement.exec() > 0;
    }

    bool DBHelper::DeleteColonist(const std::string& worldId, uint16_t colonistId)
    {
        SQLite::Statement statement(*db, R"(
            DELETE FROM colonist
            WHERE colonistId = ? AND worldId = ?
        )");

        statement.bind(1, colonistId);
        statement.bind(2, worldId);

        return statement.exec() > 0;
    }
}