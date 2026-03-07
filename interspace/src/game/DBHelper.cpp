#include "interspace/game/DBHelper.hpp"

#include "SDL3/SDL_log.h"
#include "SQLiteCpp/Statement.h"
#include "interspace/game/Typedefs.hpp"

#include <filesystem>
#include <fstream>

namespace Interspace
{
    void DBHelper::Init()
    {
        if (!worldsDb)
        {
            if (!std::filesystem::exists("data"))
                std::filesystem::create_directory("data");
            if (!std::filesystem::exists("data/server"))
                std::filesystem::create_directory("data/server");
            if (!std::filesystem::exists("data/client"))
                std::filesystem::create_directory("data/client");

            worldsDb = std::make_unique<SQLite::Database>("data/server/worlds.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
            worldsDb->exec("PRAGMA foreign_keys = ON;");
            InitWorldsDatabase();
        }

        if (!serverDb)
        {
            serverDb = std::make_unique<SQLite::Database>("data/server/server.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
            serverDb->exec("PRAGMA foreign_keys = ON;");
            InitServerDatabase();
        }

        if (!commonDb)
        {
            if (!std::filesystem::exists("data"))
                std::filesystem::create_directory("data");
            if (!std::filesystem::exists("data/shared"))
                std::filesystem::create_directory("data/shared");

            commonDb = std::make_unique<SQLite::Database>("data/shared/common.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
            commonDb->exec("PRAGMA foreign_keys = ON;");
            InitCommonDatabase();
        }

        initialized = true;
    }

    void DBHelper::InitWorldsDatabase()
    {
        worldsDb->exec(R"(
            CREATE TABLE IF NOT EXISTS world(
                worldId VARCHAR(255) PRIMARY KEY,
                worldSeed INTEGER NOT NULL,
                worldSizeX INTEGER NOT NULL,
                worldSizeY INTEGER NOT NULL,
                createdAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                lastPlayed INTEGER NOT NULL DEFAULT (0),
                gameVersion VARCHAR(50)
            );
        )");
    }

    void DBHelper::InitServerDatabase()
    {
    }

    void DBHelper::InitCommonDatabase()
    {
        commonDb->exec("DROP TABLE IF EXISTS tileData");

        commonDb->exec(R"(
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

    void DBHelper::InitWorldDatabase(SQLite::Database* db)
    {
        db->exec(R"(
            CREATE TABLE IF NOT EXISTS player(
                playerId INTEGER PRIMARY KEY,
                playerName VARCHAR(255) NOT NULL UNIQUE,
                createdAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now'))
            );
        )");

        db->exec(R"(
            CREATE TABLE IF NOT EXISTS faction(
                factionId INTEGER PRIMARY KEY,
                factionName VARCHAR(255) NOT NULL,
                factionOwner INTEGER,
                createdAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                FOREIGN KEY (factionOwner) REFERENCES player(factionOwner)
            );
        )");

        db->exec(R"(
            CREATE TABLE IF NOT EXISTS factionMember(
                factionId INTEGER NOT NULL,
                playerId INTEGER NOT NULL,
                joinedAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                PRIMARY KEY (factionId, playerId),
                FOREIGN KEY (factionId) REFERENCES faction(factionId) ON DELETE CASCADE,
                FOREIGN KEY (playerId) REFERENCES player(playerId) ON DELETE CASCADE
            );
        )");

        db->exec(R"(
            CREATE TABLE IF NOT EXISTS colonist(
                colonistId INTEGER PRIMARY KEY,
                factionId INTEGER NOT NULL,
                colonistName VARCHAR(255) NOT NULL,
                lastSeenX REAL NOT NULL,
                lastSeenY REAL NOT NULL,
                createdAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                FOREIGN KEY (factionId) REFERENCES faction(factionId)
            );
        )");
    }

    void DBHelper::CreateWorld(const std::string& worldName, uint32_t seed, uint16_t sizeX, uint16_t sizeY)
    {
        std::string serverWorldDirPath{"data/server/" + worldName};
        std::filesystem::create_directory(serverWorldDirPath);
        std::filesystem::create_directory(serverWorldDirPath + "/regions");

        std::string clientWorldDirPath{"data/client/" + worldName};
        std::filesystem::create_directory(clientWorldDirPath);
        std::filesystem::create_directory(clientWorldDirPath + "/regions");

        SQLite::Statement statement(*worldsDb, R"(
            INSERT INTO world(worldId, worldSeed, worldSizeX, worldSizeY, gameVersion)
            VALUES(?, ?, ?, ?, ?)
        )");

        statement.bind(1, worldName);
        statement.bind(2, seed);
        statement.bind(3, sizeX);
        statement.bind(4, sizeY);
        statement.bind(5, "26.1.29");

        statement.exec();
    }

    void DBHelper::LoadWorld(const std::string& worldName)
    {
        std::string worldDirPath{"data/server/" + worldName};
        if (!std::filesystem::exists(worldDirPath))
        {
            SDL_Log("Error loading world. World not found.");
            return;
        }

        if (worldDb)
            SDL_Log("World already loaded. Overwriting currently loaded world.");

        worldDb = std::make_unique<SQLite::Database>(worldDirPath + "/world.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        InitWorldDatabase(worldDb.get());
    }

    void DBHelper::UnloadWorld()
    {
        worldDb = nullptr;
    }

    bool DBHelper::AddPlayerToFaction(faction_id_t factionId, client_id_t clientId)
    {
        if (IsPlayerInFaction(factionId, clientId))
        {
            return true;
        }

        SQLite::Statement statement(*worldDb, R"(
            INSERT INTO factionMember(factionId, playerId)
            VALUES(?, ?)
        )");

        statement.bind(1, factionId);
        statement.bind(2, clientId);

        return statement.exec() > 0;
    }

    bool DBHelper::IsPlayerInFaction(faction_id_t factionId, client_id_t clientId)
    {
        SQLite::Statement query(*worldDb, R"(
            SELECT 1 FROM factionMember
            WHERE factionId = ? AND playerId = ?
        )");

        query.bind(1, factionId);
        query.bind(2, clientId);

        return query.executeStep();
    }

    bool DBHelper::IsPlayerInAnyFaction(client_id_t clientId)
    {
        SQLite::Statement query(*worldDb, R"(
            SELECT factionId FROM factionMember
            WHERE playerId = ?
        )");

        query.bind(1, clientId);

        return query.executeStep();
    }

    bool DBHelper::RemovePlayerFromFaction(faction_id_t factionId, client_id_t clientId)
    {
        SQLite::Statement statement(*worldDb, R"(
            DELETE FROM factionMember
            WHERE factionId = ? AND playerId = ?
        )");

        statement.bind(1, factionId);
        statement.bind(2, clientId);

        return statement.exec() > 0;
    }

    bool DBHelper::InsertTileData(uint32_t tileId, uint32_t tileVariant, const std::string& tileName, bool walkable, const std::string& texturePath, uint32_t atlasW, uint32_t atlasH, uint32_t atlasX, uint32_t atlasY)
    {
        SQLite::Statement statement(*commonDb, R"(
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

    bool DBHelper::InsertPlayer(client_id_t clientId, const std::string& playerName)
    {
        if (!worldDb)
            return false;

        SQLite::Statement statement(*worldDb, R"(
            INSERT INTO player(playerId, playerName)
            VALUES(?, ?)
        )");

        statement.bind(1, clientId);
        statement.bind(2, playerName);

        return statement.exec() > 0;
    }

    bool DBHelper::InsertFaction(faction_id_t factionId, const std::string& factionName, client_id_t factionOwner)
    {
        if (factionOwner == 0)
        {
            SQLite::Statement statement(*worldDb, R"(
                INSERT INTO faction(factionId, factionName)
                VALUES(?, ?)
            )");

            statement.bind(1, factionId);
            statement.bind(2, factionName);

            return statement.exec() > 0;
        }

        SQLite::Statement statement(*worldDb, R"(
            INSERT INTO faction(factionId, factionName, factionOwner)
            VALUES(?, ?, ?)
        )");

        statement.bind(1, factionId);
        statement.bind(2, factionName);
        statement.bind(3, factionOwner);

        return statement.exec() > 0;
    }

    bool DBHelper::InsertColonist(entity_id_t colonistId, faction_id_t factionId, const std::string& colonistName, float x, float y)
    {
        SQLite::Statement statement(*worldDb, R"(
            INSERT INTO colonist(colonistId, factionId, colonistName, lastSeenX, lastSeenY)
            VALUES(?, ?, ?, ?, ?)
        )");

        statement.bind(1, colonistId);
        statement.bind(2, factionId);
        statement.bind(3, colonistName);
        statement.bind(4, x);
        statement.bind(5, y);

        return statement.exec() > 0;
    }

    bool DBHelper::TileDataExists(const std::string& tileName)
    {
        SQLite::Statement query(*commonDb, "SELECT tileId FROM tileData WHERE tileName = ?");
        query.bind(1, tileName);
        return query.executeStep();
    }

    bool DBHelper::TileDataExists(uint32_t tileId, uint32_t tileVariant)
    {
        SQLite::Statement query(*commonDb, "SELECT tileId FROM tileData WHERE tileId = ? AND tileVariant = ?");
        query.bind(1, tileId);
        query.bind(2, tileVariant);
        return query.executeStep();
    }

    bool DBHelper::PlayerExists(client_id_t clientId)
    {
        if (!worldDb)
            return false;
        SQLite::Statement query(*worldDb, "SELECT playerId FROM player WHERE playerId = ?");
        query.bind(1, clientId);
        return query.executeStep();
    }

    bool DBHelper::PlayerExists(const std::string& playerName)
    {
        if (!worldDb)
            return false;

        SQLite::Statement query(*worldDb, "SELECT playerId FROM player WHERE playerName = ?");
        query.bind(1, playerName);
        return query.executeStep();
    }

    bool DBHelper::FactionExists(faction_id_t factionId)
    {
        SQLite::Statement query(*worldDb, "SELECT factionId FROM faction WHERE factionId = ?");
        query.bind(1, factionId);
        return query.executeStep();
    }

    bool DBHelper::FactionExists(const std::string& factionName)
    {
        SQLite::Statement query(*worldDb, "SELECT factionId FROM faction WHERE factionName = ?");
        query.bind(1, factionName);
        return query.executeStep();
    }

    bool DBHelper::ColonistExists(entity_id_t colonistId)
    {
        SQLite::Statement query(*worldDb, "SELECT colonistId FROM colonist WHERE colonistId = ?");
        query.bind(1, colonistId);
        return query.executeStep();
    }

    bool DBHelper::ColonistExists(const std::string& colonistName)
    {
        SQLite::Statement query(*worldDb, "SELECT colonistId FROM colonist WHERE colonistName = ?");
        query.bind(1, colonistName);
        return query.executeStep();
    }

    uint32_t DBHelper::GetTileDataId(const std::string& tileName)
    {
        SQLite::Statement query(*commonDb, "SELECT DISTINCT tileId FROM tileData WHERE tileName = ?");
        query.bind(1, tileName);

        if (query.executeStep())
        {
            return static_cast<uint32_t>(query.getColumn(0).getInt());
        }
        return 0;
    }

    std::vector<uint32_t> DBHelper::GetTileDataVariants(const std::string& tileName)
    {
        std::vector<uint32_t> tileVariants{};
        SQLite::Statement query(*commonDb, "SELECT tileVariant FROM tileData WHERE tileName = ?");
        query.bind(1, tileName);

        while (query.executeStep())
        {
            tileVariants.push_back(static_cast<uint32_t>(query.getColumn(0).getInt()));
        }
        return tileVariants;
    }

    client_id_t DBHelper::GetclientId(const std::string& playerName)
    {
        if (!worldDb)
            return 0;

        SQLite::Statement query(*worldDb, "SELECT playerId FROM player WHERE playerName = ?");
        query.bind(1, playerName);

        if (query.executeStep())
        {
            return static_cast<client_id_t>(query.getColumn(0).getInt());
        }
        return 0;
    }

    faction_id_t DBHelper::GetFactionId(const std::string& factionName)
    {
        SQLite::Statement query(*worldDb, "SELECT factionId FROM faction WHERE factionName = ?");
        query.bind(1, factionName);

        if (query.executeStep())
        {
            return static_cast<faction_id_t>(query.getColumn(0).getInt());
        }
        return 0;
    }

    std::vector<client_id_t> DBHelper::GetFactionMemberIds(faction_id_t factionId)
    {
        std::vector<client_id_t> memberIds;
        SQLite::Statement query(*worldDb, "SELECT playerId FROM factionMember WHERE factionId = ?");
        query.bind(1, factionId);

        while (query.executeStep())
        {
            memberIds.push_back(static_cast<client_id_t>(query.getColumn(0).getInt()));
        }
        return memberIds;
    }

    faction_id_t DBHelper::GetPlayerFactionId(client_id_t clientId)
    {
        SQLite::Statement query(*worldDb, "SELECT factionId FROM factionMember WHERE playerId = ? LIMIT 1");
        query.bind(1, clientId);

        if (query.executeStep())
        {
            return static_cast<faction_id_t>(query.getColumn(0).getInt());
        }
        return 0;
    }

    std::vector<faction_id_t> DBHelper::GetPlayerFactionIds(client_id_t clientId)
    {
        std::vector<faction_id_t> factionIds{};
        SQLite::Statement query(*worldDb, "SELECT factionId FROM factionMember WHERE playerId = ?");
        query.bind(1, clientId);

        while (query.executeStep())
        {
            factionIds.push_back(static_cast<faction_id_t>(query.getColumn(0).getInt()));
        }
        return factionIds;
    }
    faction_id_t DBHelper::GetColonistFactionId(entity_id_t colonistId)
    {
        SQLite::Statement query(*worldDb, "SELECT factionId FROM colonist WHERE colonistId = ?");
        query.bind(1, colonistId);

        if (query.executeStep())
        {
            return static_cast<faction_id_t>(query.getColumn(0).getInt());
        }
        return 0;
    }
    entity_id_t DBHelper::GetColonist(const std::string& colonistName)
    {
        SQLite::Statement query(*worldDb, "SELECT colonistId FROM colonist WHERE colonistName = ?");
        query.bind(1, colonistName);

        if (query.executeStep())
        {
            return static_cast<entity_id_t>(query.getColumn(0).getInt());
        }
        return 0;
    }
    Engine::Vec2<float> DBHelper::GetColonistLastSeen(entity_id_t colonistId)
    {
        SQLite::Statement query(*worldDb, "SELECT lastSeenX, lastSeenY FROM colonist WHERE colonistId = ?");
        query.bind(1, colonistId);

        if (query.executeStep())
        {
            float posX = static_cast<float>(query.getColumn(0).getDouble());
            float posY = static_cast<float>(query.getColumn(1).getDouble());
            return {posX, posY};
        }
        return {0, 0};
    }

    std::string DBHelper::GetFactionOwnerName(faction_id_t factionId)
    {
        SQLite::Statement query(*worldDb, "SELECT playerName FROM player p, faction f WHERE p.playerId = f.ownerId AND f.factionId = ?");
        query.bind(1, factionId);

        if (query.executeStep())
        {
            return query.getColumn(0).getString();
        }
        return "";
    }

    bool DBHelper::UpdateColonistLastSeen(entity_id_t colonistId, float x, float y)
    {
        SQLite::Statement statement(*worldDb, R"(
            UPDATE colonist SET lastSeenX = ?, lastSeenY = ?
            WHERE colonistId = ?
        )");

        statement.bind(1, x);
        statement.bind(2, y);
        statement.bind(3, colonistId);

        return statement.exec() > 0;
    }

    bool DBHelper::DeleteWorld(const std::string& worldName)
    {
        SQLite::Statement statement(*worldsDb, "DELETE FROM world WHERE worldId = ?");
        statement.bind(1, worldName);

        std::string serverWorldPath{"data/server/" + worldName};
        if (std::filesystem::exists(serverWorldPath))
        {
            std::filesystem::remove_all(serverWorldPath);
        }

        std::string clientWorldPath{"data/client/" + worldName};
        if (std::filesystem::exists(clientWorldPath))
        {
            std::filesystem::remove_all(clientWorldPath);
        }

        return statement.exec() > 0;
    }

    bool DBHelper::DeleteFaction(faction_id_t factionId)
    {
        SQLite::Statement statement(*worldDb, "DELETE FROM faction WHERE factionId = ?");
        statement.bind(1, factionId);
        return statement.exec() > 0;
    }

    bool DBHelper::DeleteColonist(entity_id_t colonistId)
    {
        SQLite::Statement statement(*worldDb, R"(
            DELETE FROM colonist
            WHERE colonistId = ?
        )");

        statement.bind(1, colonistId);

        return statement.exec() > 0;
    }
}