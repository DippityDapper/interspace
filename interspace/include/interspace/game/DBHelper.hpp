#pragma once

#include <vector>
#include <string>

#include "igneous/Vec2.hpp"
#include "SQLiteCpp/Database.h"

namespace Interspace
{
    class DBHelper
    {
    public:
        static inline std::unique_ptr<SQLite::Database> db = nullptr;

    public:
        static void InitDatabase();

        // ============================================================
        // EXISTS CHECKS
        // ============================================================
        static bool WorldExists(const std::string& worldId);
        static bool ChunkExists(const std::string& worldId, int32_t chunkX, int32_t chunkY);
        static bool TileExists(const std::string& worldId, int32_t chunkX, int32_t chunkY, int32_t tileX, int32_t tileY);
        static bool PlayerExists(uint32_t playerId);
        static bool PlayerExistsByName(const std::string& playerName);
        static bool FactionExists(uint16_t factionId, const std::string& worldId);
        static bool FactionExistsByName(const std::string& worldId, const std::string& factionName);
        static bool ColonistExists(const std::string& worldId, uint16_t colonistId);
        static bool ColonistExistsByName(const std::string& worldId, const std::string& colonistName);
        static bool TileTypeExists(int32_t tileId);
        static bool TileTypeExistsByName(const std::string& tileName);

        // ============================================================
        // GET METHODS
        // ============================================================
        static uint16_t GetFactionIdByName(const std::string& worldId, const std::string& factionName);
        static std::vector<uint32_t> GetFactionMemberIds(const std::string& worldId, uint16_t factionId);
        static uint16_t GetPlayerFactionId(const std::string& worldId, uint32_t playerId);
        static std::vector<uint16_t> GetPlayerFactionIds(const std::string& worldId, uint32_t playerId);
        static uint16_t GetColonistFactionId(const std::string& worldId, uint16_t colonistId);
        static uint16_t GetColonistByName(const std::string& worldId, const std::string& colonistName);
        static Engine::Vec2<float> GetColonistPosition(const std::string& worldId, uint16_t colonistId);
        static uint32_t GetPlayerIdByName(const std::string& playerName);

        // ============================================================
        // INSERT METHODS
        // ============================================================
        static bool InsertWorld(const std::string& worldId, uint32_t worldSeed, uint16_t worldSizeX, uint16_t worldSizeY, const std::string& gameVersion = "");
        static bool InsertChunk(const std::string& worldId, int32_t chunkX, int32_t chunkY);
        static bool InsertTile(const std::string& worldId, int32_t chunkX, int32_t chunkY, int32_t tileX, int32_t tileY, int32_t tileType);
        static bool InsertPlayer(uint32_t playerId, const std::string& playerName);
        static bool InsertFaction(uint16_t factionId, const std::string& worldId, const std::string& factionName, uint32_t factionOwner);
        static bool InsertColonist(uint16_t colonistId, const std::string& worldId, uint16_t factionId, const std::string& colonistName, float x, float y);
        static bool InsertTileType(int32_t tileId, const std::string& tileName, bool walkable, const std::string& textureId);

        // ============================================================
        // UPDATE METHODS
        // ============================================================
        static bool UpdateWorld(const std::string& worldId, int32_t worldSeed, int32_t worldSizeX, int32_t worldSizeY);
        static bool UpdateWorldLastPlayed(const std::string& worldId);
        static bool UpdateTile(const std::string& worldId, int32_t chunkX, int32_t chunkY, int32_t tileX, int32_t tileY, int32_t tileType);
        static bool UpdatePlayerName(uint32_t playerId, const std::string& playerName);
        static bool UpdateFactionName(uint16_t factionId, const std::string& factionName);
        static bool UpdateFactionOwner(uint16_t factionId, uint32_t factionOwner);
        static bool UpdateColonistPosition(const std::string& worldId, uint16_t colonistId, float x, float y);
        static bool UpdateColonistFaction(const std::string& worldId, uint16_t colonistId, uint16_t factionId);
        static bool UpdateColonistName(const std::string& worldId, uint16_t colonistId, const std::string& colonistName);
        static bool UpdateTileType(int32_t tileId, const std::string& tileName, bool walkable, const std::string& textureId);

        // ============================================================
        // RELATIONSHIP METHODS
        // ============================================================
        static bool IsPlayerInFaction(const std::string& worldId, uint16_t factionId, uint32_t playerId);
        static bool IsPlayerInAnyFaction(const std::string& worldId, uint32_t playerId);
        static bool AddPlayerToFaction(const std::string& worldId, uint16_t factionId, uint32_t playerId);
        static bool RemovePlayerFromFaction(const std::string& worldId, uint16_t factionId, uint32_t playerId);

        // ============================================================
        // DELETE METHODS
        // ============================================================
        static bool DeleteWorld(const std::string& worldId);
        static bool DeleteChunk(const std::string& worldId, int32_t chunkX, int32_t chunkY);
        static bool DeleteTile(const std::string& worldId, int32_t chunkX, int32_t chunkY, int32_t tileX, int32_t tileY);
        static bool DeletePlayer(uint32_t playerId);
        static bool DeleteFaction(uint16_t factionId);
        static bool DeleteColonist(const std::string& worldId, uint16_t colonistId);
        static bool DeleteTileType(int32_t tileId);
    };
}