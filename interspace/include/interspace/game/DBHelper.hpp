#pragma once

#include "SQLiteCpp/Backup.h"
#include "igneous/engine/Vec2.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Interspace
{
    class DBHelper
    {
      public:
        static inline std::unique_ptr<SQLite::Database> worldsDb = nullptr;
        static inline std::unique_ptr<SQLite::Database> worldDb = nullptr;
        static inline std::unique_ptr<SQLite::Database> serverDb = nullptr;
        static inline std::unique_ptr<SQLite::Database> commonDb = nullptr;

        static inline bool initialized = false;

      private:
        //-------------------------------
        // CREATE TABLES
        //-------------------------------

        static void InitWorldsDatabase();
        static void InitServerDatabase();
        static void InitCommonDatabase();
        static void InitWorldDatabase(SQLite::Database* db);

      public:
        static void Init();

        static void CreateWorld(const std::string& worldName, uint32_t seed, uint16_t sizeX, uint16_t sizeY);
        static void LoadWorld(const std::string& worldName);
        static void UnloadWorld();

        static bool AddPlayerToFaction(uint16_t factionId, uint32_t playerId);
        static bool IsPlayerInFaction(uint16_t factionId, uint32_t playerId);
        static bool IsPlayerInAnyFaction(uint32_t playerId);
        static bool RemovePlayerFromFaction(uint16_t factionId, uint32_t playerId);

        //-------------------------------
        // INSERT DATA
        //-------------------------------

        static bool InsertTileData(uint32_t tileId, uint32_t tileVariant, const std::string& tileName, bool walkable, const std::string& texturePath, uint32_t atlasW, uint32_t atlasH, uint32_t atlasX, uint32_t atlasY);
        static bool InsertPlayer(uint32_t playerId, const std::string& playerName);
        static bool InsertFaction(uint16_t factionId, const std::string& factionName, uint32_t factionOwner);
        static bool InsertColonist(uint32_t colonistId, uint16_t factionId, const std::string& colonistName, float x, float y);

        //-------------------------------
        // DATA EXISTS
        //-------------------------------

        static bool TileDataExists(const std::string& tileName);
        static bool TileDataExists(uint32_t tileId, uint32_t tileVariant);
        static bool PlayerExists(uint32_t playerId);
        static bool PlayerExists(const std::string& playerName);
        static bool FactionExists(uint16_t factionId);
        static bool FactionExists(const std::string& factionName);
        static bool ColonistExists(uint32_t colonistId);
        static bool ColonistExists(const std::string& colonistName);

        //-------------------------------
        // GET DATA
        //-------------------------------

        static uint32_t GetTileDataId(const std::string& tileName);
        static std::vector<uint32_t> GetTileDataVariants(const std::string& tileName);
        static uint32_t GetPlayerId(const std::string& playerName);
        static uint16_t GetFactionId(const std::string& factionName);
        static std::vector<uint32_t> GetFactionMemberIds(uint16_t factionId);
        static uint16_t GetPlayerFactionId(uint32_t playerId);
        static std::vector<uint16_t> GetPlayerFactionIds(uint32_t playerId);
        static uint16_t GetColonistFactionId(uint32_t colonistId);
        static uint32_t GetColonist(const std::string& colonistName);
        static Engine::Vec2<float> GetColonistLastSeen(uint32_t colonistId);

        //-------------------------------
        // UPDATE DATA
        //-------------------------------

        static bool UpdateColonistLastSeen(uint32_t colonistId, float x, float y);

        //-------------------------------
        // DELETE DATA
        //-------------------------------

        static bool DeleteWorld(const std::string& worldName);
        static bool DeleteFaction(uint16_t factionId);
        static bool DeleteColonist(uint32_t colonistId);
    };
}