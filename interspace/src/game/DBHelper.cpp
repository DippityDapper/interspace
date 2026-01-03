#include "interspace/game/DBHelper.hpp"

#include "igneous/Database.hpp"

namespace Interspace
{
    void DBHelper::InitDatabase()
    {
        db = Engine::Database::Open("worlds.db");
        Engine::Database::Execute(db.get(), "PRAGMA foreign_keys = ON;");

        Engine::Database::Execute(db.get(),
            R"(
                CREATE TABLE IF NOT EXISTS world(
                    worldId VARCHAR(255) PRIMARY KEY,
                    worldSeed INTEGER NOT NULL,
                    worldSizeX INTEGER NOT NULL,
                    worldSizeY INTEGER NOT NULL,
                    createdAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                    lastPlayed INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                    gameVersion VARCHAR(50)
                );
            )"
        );

        Engine::Database::Execute(db.get(),
            R"(
                CREATE TABLE IF NOT EXISTS chunk(
                    worldId VARCHAR(255) NOT NULL,
                    chunkX INTEGER NOT NULL,
                    chunkY INTEGER NOT NULL,
                    PRIMARY KEY (worldId, chunkX, chunkY),
                    FOREIGN KEY (worldId) REFERENCES world(worldId) ON DELETE CASCADE
                );
            )"
        );

        Engine::Database::Execute(db.get(),
            R"(
                CREATE TABLE IF NOT EXISTS tileType(
                    tileId INTEGER PRIMARY KEY,
                    tileName VARCHAR(255) NOT NULL UNIQUE,
                    walkable INTEGER NOT NULL DEFAULT 1,
                    textureId VARCHAR(255)
                );
            )"
        );

        Engine::Database::Execute(db.get(),
            R"(
                CREATE TABLE IF NOT EXISTS tile(
                    worldId VARCHAR(255) NOT NULL,
                    chunkX INTEGER NOT NULL,
                    chunkY INTEGER NOT NULL,
                    tileX INTEGER NOT NULL,
                    tileY INTEGER NOT NULL,
                    tileType INTEGER NOT NULL,
                    PRIMARY KEY (worldId, chunkX, chunkY, tileX, tileY),
                    FOREIGN KEY (worldId, chunkX, chunkY) REFERENCES chunk(worldId, chunkX, chunkY) ON DELETE CASCADE,
                    FOREIGN KEY (tileType) REFERENCES tileType(tileId) ON DELETE RESTRICT
                );
            )"
        );

        Engine::Database::Execute(db.get(),
            R"(
                CREATE TABLE IF NOT EXISTS player(
                    playerId INTEGER PRIMARY KEY,
                    playerName VARCHAR(255) NOT NULL UNIQUE,
                    createdAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now'))
                );
            )"
        );

        Engine::Database::Execute(db.get(),
            R"(
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
            )"
        );

        Engine::Database::Execute(db.get(),
            R"(
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
            )"
        );

        Engine::Database::Execute(db.get(),
            R"(
                CREATE TABLE IF NOT EXISTS factionMember(
                    factionId INTEGER NOT NULL,
                    worldId VARCHAR(255) NOT NULL,
                    playerId INTEGER NOT NULL,
                    joinedAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                    PRIMARY KEY (factionId, worldId, playerId),
                    FOREIGN KEY (factionId, worldId) REFERENCES faction(factionId, worldId) ON DELETE CASCADE
                    FOREIGN KEY (playerId) REFERENCES player(playerId) ON DELETE CASCADE
                );
            )"
        );

        Engine::Database::Execute(db.get(),
            R"(
                CREATE INDEX IF NOT EXISTS idx_tile_chunk_lookup
                ON tile(worldId, chunkX, chunkY);
            )"
        );

        Engine::Database::Execute(db.get(),
            R"(
                CREATE INDEX IF NOT EXISTS idx_colonist_world_position
                ON colonist(worldId, colonistX, colonistY);
            )"
        );

        Engine::Database::Execute(db.get(),
            R"(
                CREATE INDEX IF NOT EXISTS idx_colonist_faction
                ON colonist(factionId);
            )"
        );

        Engine::Database::Execute(db.get(),
            R"(
                CREATE INDEX IF NOT EXISTS idx_faction_world
                ON faction(worldId);
            )"
        );

        Engine::Database::Execute(db.get(),
            R"(
                CREATE INDEX IF NOT EXISTS idx_faction_member_player
                ON factionMember(playerId);
            )"
        );
    }

    // ============================================================
    // EXISTS CHECKS
    // ============================================================

    bool DBHelper::WorldExists(const std::string& worldId)
    {
        std::string query = "SELECT worldId FROM world WHERE worldId = '" + worldId + "'";
        auto result = Engine::Database::Query(db.get(), query);
        return !result.empty();
    }

    bool DBHelper::ChunkExists(const std::string& worldId, int32_t chunkX, int32_t chunkY)
    {
        std::string query = "SELECT * FROM chunk WHERE worldId = '" + worldId +
            "' AND chunkX = " + std::to_string(chunkX) +
            " AND chunkY = " + std::to_string(chunkY);
        auto result = Engine::Database::Query(db.get(), query);
        return !result.empty();
    }

    bool DBHelper::TileExists(const std::string& worldId, int32_t chunkX, int32_t chunkY, int32_t tileX, int32_t tileY)
    {
        std::string query = "SELECT * FROM tile WHERE worldId = '" + worldId +
            "' AND chunkX = " + std::to_string(chunkX) +
            " AND chunkY = " + std::to_string(chunkY) +
            " AND tileX = " + std::to_string(tileX) +
            " AND tileY = " + std::to_string(tileY);
        auto result = Engine::Database::Query(db.get(), query);
        return !result.empty();
    }

    bool DBHelper::PlayerExists(uint32_t playerId)
    {
        std::string query = "SELECT playerId FROM player WHERE playerId = " + std::to_string(playerId);
        auto result = Engine::Database::Query(db.get(), query);
        return !result.empty();
    }

    bool DBHelper::PlayerExistsByName(const std::string& playerName)
    {
        std::string query = "SELECT playerId FROM player WHERE playerName = '" + playerName + "'";
        auto result = Engine::Database::Query(db.get(), query);
        return !result.empty();
    }

    bool DBHelper::FactionExists(uint16_t factionId, const std::string& worldId)
    {
        std::string query = "SELECT factionId FROM faction WHERE factionId = " + std::to_string(factionId) +
            " AND worldId = '" + worldId + "'";
        auto result = Engine::Database::Query(db.get(), query);
        return !result.empty();
    }

    bool DBHelper::FactionExistsByName(const std::string& worldId, const std::string& factionName)
    {
        std::string query = "SELECT factionId FROM faction WHERE factionName = '" +
            factionName + "' AND worldId = '" + worldId + "'";
        auto result = Engine::Database::Query(db.get(), query);
        return !result.empty();
    }

    bool DBHelper::ColonistExists(const std::string& worldId, uint16_t colonistId)
    {
        std::string query = "SELECT colonistId FROM colonist WHERE colonistId = " +
            std::to_string(colonistId) + " AND worldId = '" + worldId + "'";
        auto result = Engine::Database::Query(db.get(), query);
        return !result.empty();
    }

    bool DBHelper::ColonistExistsByName(const std::string& worldId, const std::string& colonistName)
    {
        std::string query = "SELECT colonistId FROM colonist WHERE colonistName = '" +
            colonistName + "' AND worldId = '" + worldId + "'";
        auto result = Engine::Database::Query(db.get(), query);
        return !result.empty();
    }

    bool DBHelper::TileTypeExists(int32_t tileId)
    {
        std::string query = "SELECT tileId FROM tileType WHERE tileId = " + std::to_string(tileId);
        auto result = Engine::Database::Query(db.get(), query);
        return !result.empty();
    }

    bool DBHelper::TileTypeExistsByName(const std::string& tileName)
    {
        std::string query = "SELECT tileId FROM tileType WHERE tileName = '" + tileName + "'";
        auto result = Engine::Database::Query(db.get(), query);
        return !result.empty();
    }

    // ============================================================
    // GET METHODS
    // ============================================================

    uint16_t DBHelper::GetFactionIdByName(const std::string& worldId, const std::string& factionName)
    {
        std::string query = "SELECT factionId FROM faction WHERE factionName = '" +
            factionName + "' AND worldId = '" + worldId + "'";
        auto result = Engine::Database::Query(db.get(), query);
        if (!result.empty())
        {
            return static_cast<uint16_t>(std::stoul(result[0]["factionId"]));
        }
        return 0;
    }

    std::vector<uint32_t> DBHelper::GetFactionMemberIds(const std::string& worldId, uint16_t factionId)
    {
        std::vector<uint32_t> memberIds;
        std::string query = "SELECT playerId FROM factionMember WHERE factionId = " + std::to_string(factionId) +
            " AND worldId = '" + worldId + "'";
        auto result = Engine::Database::Query(db.get(), query);
        if (!result.empty())
        {
            for (const auto& row : result)
            {
                memberIds.push_back(std::stoul(row.at("playerId")));
            }
        }
        return memberIds;
    }

    uint16_t DBHelper::GetPlayerFactionId(const std::string& worldId, uint32_t playerId)
    {
        std::string query = "SELECT factionId FROM factionMember WHERE worldId = '" + worldId +
            "' AND playerId = " + std::to_string(playerId) + " LIMIT 1";
        auto result = Engine::Database::Query(db.get(), query);
        if (!result.empty())
        {
            return static_cast<uint16_t>(std::stoul(result[0]["factionId"]));
        }
        return 0;
    }

    std::vector<uint16_t> DBHelper::GetPlayerFactionIds(const std::string& worldId, uint32_t playerId)
    {
        std::vector<uint16_t> factionIds;
        std::string query = "SELECT factionId FROM factionMember WHERE worldId = '" + worldId +
            "' AND playerId = " + std::to_string(playerId);
        auto result = Engine::Database::Query(db.get(), query);
        if (!result.empty())
        {
            for (const auto& row : result)
            {
                factionIds.push_back(static_cast<uint16_t>(std::stoul(row.at("factionId"))));
            }
        }
        return factionIds;
    }

    uint16_t DBHelper::GetColonistFactionId(const std::string& worldId, uint16_t colonistId)
    {
        std::string query = "SELECT factionId FROM colonist WHERE colonistId = " +
            std::to_string(colonistId) + " AND worldId = '" + worldId + "'";
        auto result = Engine::Database::Query(db.get(), query);
        if (!result.empty())
        {
            return static_cast<uint16_t>(std::stoul(result[0]["factionId"]));
        }
        return 0;
    }

    uint16_t DBHelper::GetColonistByName(const std::string& worldId, const std::string& colonistName)
    {
        std::string query = "SELECT colonistId FROM colonist WHERE colonistName = '" +
            colonistName + "' AND worldId = '" + worldId + "'";
        auto result = Engine::Database::Query(db.get(), query);
        if (!result.empty())
        {
            return static_cast<uint16_t>(std::stoul(result[0]["colonistId"]));
        }
        return 0;
    }

    Engine::Vec2<float> DBHelper::GetColonistPosition(const std::string& worldId, uint16_t colonistId)
    {
        std::string query = "SELECT colonistX, colonistY FROM colonist WHERE colonistId = " +
            std::to_string(colonistId) + " AND worldId = '" + worldId + "'";
        auto result = Engine::Database::Query(db.get(), query);
        if (!result.empty())
        {
            float posX = std::stof(result[0]["colonistX"]);
            float posY = std::stof(result[0]["colonistY"]);
            return {posX, posY};
        }
        return {0, 0};
    }

    uint32_t DBHelper::GetPlayerIdByName(const std::string& playerName)
    {
        std::string query = "SELECT playerId FROM player WHERE playerName = '" +
        playerName + "'";
        auto result = Engine::Database::Query(db.get(), query);
        if (!result.empty())
        {
            return static_cast<uint32_t>(std::stoul(result[0]["playerId"]));
        }
        return 0;
    }

    // ============================================================
    // INSERT METHODS
    // ============================================================

    bool DBHelper::InsertWorld(const std::string& worldId, uint32_t worldSeed, uint16_t worldSizeX, uint16_t worldSizeY, const std::string& gameVersion)
    {
        std::string query = "INSERT INTO world(worldId, worldSeed, worldSizeX, worldSizeY, gameVersion) VALUES('" +
            worldId + "', " +
            std::to_string(worldSeed) + ", " +
            std::to_string(worldSizeX) + ", " + std::to_string(worldSizeY) + ", '" +
            gameVersion + "')";
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::InsertChunk(const std::string& worldId, int32_t chunkX, int32_t chunkY)
    {
        std::string query = "INSERT INTO chunk(worldId, chunkX, chunkY) VALUES('" +
            worldId + "', " + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ")";
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::InsertTile(const std::string& worldId, int32_t chunkX, int32_t chunkY, int32_t tileX, int32_t tileY, int32_t tileType)
    {
        std::string query = "INSERT INTO tile(worldId, chunkX, chunkY, tileX, tileY, tileType) VALUES('" +
            worldId + "', " + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ", " +
            std::to_string(tileX) + ", " + std::to_string(tileY) + ", " + std::to_string(tileType) + ")";
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::InsertPlayer(uint32_t playerId, const std::string& playerName)
    {
        std::string query = "INSERT INTO player(playerId, playerName) VALUES(" + std::to_string(playerId) + ",'" + playerName + "')";
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::InsertFaction(uint16_t factionId, const std::string& worldId, const std::string& factionName, uint32_t factionOwner)
    {
        std::string query = "INSERT INTO faction(factionId, worldId, factionName, factionOwner) VALUES(" +
            std::to_string(factionId) + ", '" +
            worldId + "', '" +
            factionName + "', " +
            std::to_string(factionOwner) + ")";

        if (factionOwner == 0)
        {
            query = "INSERT INTO faction(factionId, worldId, factionName) VALUES(" +
            std::to_string(factionId) + ", '" +
            worldId + "', '" +
            factionName + "')";
        }

        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::InsertColonist(uint16_t colonistId, const std::string& worldId, uint16_t factionId, const std::string& colonistName, float x, float y)
    {
        std::string query = "INSERT INTO colonist(colonistId, worldId, factionId, colonistName, colonistX, colonistY) VALUES(" +
            std::to_string(colonistId) + ", '" +
            worldId + "', " +
            std::to_string(factionId) + ", '" +
            colonistName + "', " +
            std::to_string(x) + ", " + std::to_string(y) + ")";
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::InsertTileType(int32_t tileId, const std::string& tileName, bool walkable, const std::string& textureId)
    {
        std::string query = "INSERT INTO tileType(tileId, tileName, walkable, textureId) VALUES(" +
            std::to_string(tileId) + ", '" + tileName + "', " +
            std::to_string(walkable ? 1 : 0) + ", '" + textureId + "')";
        return Engine::Database::Execute(db.get(), query);
    }

    // ============================================================
    // UPDATE METHODS
    // ============================================================

    bool DBHelper::UpdateWorld(const std::string& worldId, int32_t worldSeed, int32_t worldSizeX, int32_t worldSizeY)
    {
        std::string query = "UPDATE world SET worldSeed = " + std::to_string(worldSeed) +
            ", worldSizeX = " + std::to_string(worldSizeX) +
            ", worldSizeY = " + std::to_string(worldSizeY) +
            " WHERE worldId = '" + worldId + "'";
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::UpdateWorldLastPlayed(const std::string& worldId)
    {
        std::string query = "UPDATE world SET lastPlayed = strftime('%s', 'now') WHERE worldId = '" + worldId + "'";
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::UpdateTile(const std::string& worldId, int32_t chunkX, int32_t chunkY, int32_t tileX, int32_t tileY, int32_t tileType)
    {
        std::string query = "UPDATE tile SET tileType = " + std::to_string(tileType) +
            " WHERE worldId = '" + worldId + "' AND chunkX = " + std::to_string(chunkX) +
            " AND chunkY = " + std::to_string(chunkY) + " AND tileX = " + std::to_string(tileX) +
            " AND tileY = " + std::to_string(tileY);
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::UpdatePlayerName(uint32_t playerId, const std::string& playerName)
    {
        std::string query = "UPDATE player SET playerName = '" + playerName +
            "' WHERE playerId = " + std::to_string(playerId);
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::UpdateFactionName(uint16_t factionId, const std::string& factionName)
    {
        std::string query = "UPDATE faction SET factionName = '" + factionName +
            "' WHERE factionId = " + std::to_string(factionId);
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::UpdateFactionOwner(uint16_t factionId, uint32_t factionOwner)
    {
        std::string query = "UPDATE faction SET factionOwner = " + std::to_string(factionOwner) +
            " WHERE factionId = " + std::to_string(factionId);
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::UpdateColonistPosition(const std::string& worldId, uint16_t colonistId, float x, float y)
    {
        std::string query = "UPDATE colonist SET colonistX = " + std::to_string(x) +
            ", colonistY = " + std::to_string(y) +
            " WHERE colonistId = " + std::to_string(colonistId) +
            " AND worldId = '" + worldId + "'";
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::UpdateColonistFaction(const std::string& worldId, uint16_t colonistId, uint16_t factionId)
    {
        std::string query = "UPDATE colonist SET factionId = " + std::to_string(factionId) +
            " WHERE colonistId = " + std::to_string(colonistId) +
            " AND worldId = '" + worldId + "'";
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::UpdateColonistName(const std::string& worldId, uint16_t colonistId, const std::string& colonistName)
    {
        std::string query = "UPDATE colonist SET colonistName = '" + colonistName +
            "' WHERE colonistId = " + std::to_string(colonistId) +
            " AND worldId = '" + worldId + "'";
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::UpdateTileType(int32_t tileId, const std::string& tileName, bool walkable, const std::string& textureId)
    {
        std::string query = "UPDATE tileType SET tileName = '" + tileName +
            "', walkable = " + std::to_string(walkable ? 1 : 0) +
            ", textureId = '" + textureId +
            "' WHERE tileId = " + std::to_string(tileId);
        return Engine::Database::Execute(db.get(), query);
    }

    // ============================================================
    // RELATIONSHIP METHODS
    // ============================================================

    bool DBHelper::IsPlayerInFaction(const std::string& worldId, uint16_t factionId, uint32_t playerId)
    {
        std::string query = "SELECT * FROM factionMember WHERE worldId = '" + worldId +
            "' AND factionId = " + std::to_string(factionId) +
            " AND playerId = " + std::to_string(playerId);
        auto result = Engine::Database::Query(db.get(), query);
        return !result.empty();
    }

    bool DBHelper::IsPlayerInAnyFaction(const std::string& worldId, uint32_t playerId)
    {
        std::string query = "SELECT factionId FROM factionMember WHERE worldId = '" + worldId +
            "' AND playerId = " + std::to_string(playerId);
        auto result = Engine::Database::Query(db.get(), query);
        return !result.empty();
    }

    bool DBHelper::AddPlayerToFaction(const std::string& worldId, uint16_t factionId, uint32_t playerId)
    {
        if (IsPlayerInFaction(worldId, factionId, playerId))
        {
            return true;
        }
        std::string query = "INSERT INTO factionMember(worldId, factionId, playerId) VALUES('" +
            worldId + "', " +
            std::to_string(factionId) + ", " +
            std::to_string(playerId) + ")";
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::RemovePlayerFromFaction(const std::string& worldId, uint16_t factionId, uint32_t playerId)
    {
        std::string query = "DELETE FROM factionMember WHERE worldId = '" + worldId +
            "' AND factionId = " + std::to_string(factionId) +
            " AND playerId = " + std::to_string(playerId);
        return Engine::Database::Execute(db.get(), query);
    }

    // ============================================================
    // DELETE METHODS
    // ============================================================

    bool DBHelper::DeleteWorld(const std::string& worldId)
    {
        std::string query = "DELETE FROM world WHERE worldId = '" + worldId + "'";
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::DeleteChunk(const std::string& worldId, int32_t chunkX, int32_t chunkY)
    {
        std::string query = "DELETE FROM chunk WHERE worldId = '" + worldId +
            "' AND chunkX = " + std::to_string(chunkX) +
            " AND chunkY = " + std::to_string(chunkY);
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::DeleteTile(const std::string& worldId, int32_t chunkX, int32_t chunkY, int32_t tileX, int32_t tileY)
    {
        std::string query = "DELETE FROM tile WHERE worldId = '" + worldId +
            "' AND chunkX = " + std::to_string(chunkX) +
            " AND chunkY = " + std::to_string(chunkY) +
            " AND tileX = " + std::to_string(tileX) +
            " AND tileY = " + std::to_string(tileY);
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::DeletePlayer(uint32_t playerId)
    {
        std::string query = "DELETE FROM player WHERE playerId = " + std::to_string(playerId);
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::DeleteFaction(uint16_t factionId)
    {
        std::string query = "DELETE FROM faction WHERE factionId = " + std::to_string(factionId);
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::DeleteColonist(const std::string& worldId, uint16_t colonistId)
    {
        std::string query = "DELETE FROM colonist WHERE colonistId = " + std::to_string(colonistId) +
            " AND worldId = '" + worldId + "'";
        return Engine::Database::Execute(db.get(), query);
    }

    bool DBHelper::DeleteTileType(int32_t tileId)
    {
        std::string query = "DELETE FROM tileType WHERE tileId = " + std::to_string(tileId);
        return Engine::Database::Execute(db.get(), query);
    }
}