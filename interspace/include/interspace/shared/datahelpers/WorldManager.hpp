#pragma once
#include "SQLiteCpp/Database.h"
#include "interspace/shared/game/Typedefs.hpp"
#include "interspace/shared/world/WorldData.hpp"
#include "interspace/shared/world/generators/WorldGenerator.hpp"

#include <unordered_map>
#include <vector>

namespace Interspace
{
    class WorldManager
    {
      private:
        static inline universe_id_t loadedUniverseId = 0;
        static inline std::unordered_map<world_id_t, std::unique_ptr<WorldData>> worldsData{};
        static inline std::unordered_map<WorldType, std::unique_ptr<WorldGenerator>> worldGenerators{};

      public:
        static void Init();

        static void CreateTables();
        static void CheckAndCreateFiles(const std::string& universeName);

        static void LoadWorlds(const std::string& universeName);
        static bool InsertWorld(const std::string& universeName, WorldType worldType, uint16_t worldSizeX, uint16_t worldSizeY);
        static bool DeleteWorld(const std::string& universeName, world_id_t worldId);

        static void AddWorld(world_id_t worldId, WorldType worldType, uint32_t worldSeed, uint16_t worldSizeX, uint16_t worldSizeY);
        static void RemoveWorld(world_id_t worldId);

        static std::vector<WorldData*> GetWorlds();
        static WorldData* GetWorldData(world_id_t worldId);
        static WorldGenerator* GetWorldGenerator(WorldType worldType);
        static void GenerateChunk(ChunkData* chunk, WorldType worldType);
    };
}