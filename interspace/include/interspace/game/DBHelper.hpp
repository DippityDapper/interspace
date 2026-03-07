#pragma once

#include "Typedefs.hpp"
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

        static bool AddPlayerToFaction(faction_id_t factionId, client_id_t clientId);
        static bool IsPlayerInFaction(faction_id_t factionId, client_id_t clientId);
        static bool IsPlayerInAnyFaction(client_id_t clientId);
        static bool RemovePlayerFromFaction(faction_id_t factionId, client_id_t clientId);

        //-------------------------------
        // INSERT DATA
        //-------------------------------

        static bool InsertTileData(uint32_t tileId, uint32_t tileVariant, const std::string& tileName, bool walkable, const std::string& texturePath, uint32_t atlasW, uint32_t atlasH, uint32_t atlasX, uint32_t atlasY);
        static bool InsertPlayer(client_id_t clientId, const std::string& playerName);
        static bool InsertFaction(faction_id_t factionId, const std::string& factionName, client_id_t factionOwner);
        static bool InsertColonist(entity_id_t colonistId, faction_id_t factionId, const std::string& colonistName, float x, float y);

        //-------------------------------
        // DATA EXISTS
        //-------------------------------

        static bool TileDataExists(const std::string& tileName);
        static bool TileDataExists(uint32_t tileId, uint32_t tileVariant);
        static bool PlayerExists(client_id_t clientId);
        static bool PlayerExists(const std::string& playerName);
        static bool FactionExists(faction_id_t factionId);
        static bool FactionExists(const std::string& factionName);
        static bool ColonistExists(entity_id_t colonistId);
        static bool ColonistExists(const std::string& colonistName);

        //-------------------------------
        // GET DATA
        //-------------------------------

        static uint32_t GetTileDataId(const std::string& tileName);
        static std::vector<uint32_t> GetTileDataVariants(const std::string& tileName);
        static client_id_t GetclientId(const std::string& playerName);
        static faction_id_t GetFactionId(const std::string& factionName);
        static std::vector<client_id_t> GetFactionMemberIds(faction_id_t factionId);
        static faction_id_t GetPlayerFactionId(client_id_t clientId);
        static std::vector<faction_id_t> GetPlayerFactionIds(client_id_t clientId);
        static faction_id_t GetColonistFactionId(entity_id_t colonistId);
        static entity_id_t GetColonist(const std::string& colonistName);
        static Engine::Vec2<float> GetColonistLastSeen(entity_id_t colonistId);
        static std::string GetFactionOwnerName(faction_id_t factionId);

        //-------------------------------
        // UPDATE DATA
        //-------------------------------

        static bool UpdateColonistLastSeen(entity_id_t colonistId, float x, float y);

        //-------------------------------
        // DELETE DATA
        //-------------------------------

        static bool DeleteWorld(const std::string& worldName);
        static bool DeleteFaction(faction_id_t factionId);
        static bool DeleteColonist(entity_id_t colonistId);
    };
}