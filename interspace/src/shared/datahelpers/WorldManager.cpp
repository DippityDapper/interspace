#include "interspace/shared/datahelpers/WorldManager.hpp"

#include "interspace/shared/datahelpers/DatabaseManager.hpp"
#include "interspace/shared/datahelpers/UniverseManager.hpp"
#include "interspace/shared/world/generators/TestGenerator.hpp"

#include <optional>
#include <random>

namespace Interspace
{
    void WorldManager::Init()
    {
        CreateTables();
        worldGenerators.emplace(Test, std::make_unique<TestGenerator>());
    }

    void WorldManager::LoadWorlds(const std::string& universeName)
    {
        if (!std::filesystem::exists("data"))
            std::filesystem::create_directory("data");
        if (!std::filesystem::exists("data/shared"))
            std::filesystem::create_directory("data/shared");

        loadedUniverseId = UniverseManager::GetUniverseId(universeName);

        worldsData.clear();

        SQLite::Database* db = DatabaseManager::GetSharedDatabase();
        SQLite::Statement statement(*db, R"(
            SELECT worldId, worldType, worldSeed, worldSizeX, worldSizeY FROM world WHERE universeId = ?
        )");

        statement.bind(1, loadedUniverseId);

        while (statement.executeStep())
        {
            world_id_t worldId = static_cast<world_id_t>(statement.getColumn(0).getInt());
            WorldType worldType = static_cast<WorldType>(statement.getColumn(1).getInt());
            uint32_t worldSeed = static_cast<uint32_t>(statement.getColumn(2).getInt());
            uint16_t worldSizeX = static_cast<uint16_t>(statement.getColumn(3).getInt());
            uint16_t worldSizeY = static_cast<uint16_t>(statement.getColumn(4).getInt());

            worldsData.emplace(worldId, std::make_unique<WorldData>(worldId, worldType, worldSeed, worldSizeX, worldSizeY));
        }
    }

    void WorldManager::CreateTables()
    {
        SQLite::Database* db = DatabaseManager::GetSharedDatabase();

        db->exec(R"(
            CREATE TABLE IF NOT EXISTS world(
                worldId INTEGER NOT NULL,
                universeId INTEGER NOT NULL,
                worldType INTEGER NOT NULL,
                worldSeed INTEGER NOT NULL,
                worldSizeX INTEGER NOT NULL,
                worldSizeY INTEGER NOT NULL,
                createdAt INTEGER NOT NULL DEFAULT (strftime('%s', 'now')),
                gameVersion VARCHAR(50),
                PRIMARY KEY (worldId, universeId),
                FOREIGN KEY (universeId) REFERENCES universe(universeId) ON DELETE CASCADE
            );
        )");
    }

    void WorldManager::CheckAndCreateFiles(const std::string& universeName)
    {
        if (!std::filesystem::exists("data"))
            std::filesystem::create_directory("data");
        if (!std::filesystem::exists("data/server"))
            std::filesystem::create_directory("data/server");
        if (!std::filesystem::exists("data/server/universes"))
            std::filesystem::create_directory("data/server/universes");
        if (!std::filesystem::exists("data/server/universes/" + universeName))
            std::filesystem::create_directory("data/server/universes/" + universeName);
        if (!std::filesystem::exists("data/server/universes/" + universeName + "/worlds"))
            std::filesystem::create_directory("data/server/universes/" + universeName + "/worlds");

        if (!std::filesystem::exists("data/client"))
            std::filesystem::create_directory("data/client");
        if (!std::filesystem::exists("data/client/universes"))
            std::filesystem::create_directory("data/client/universes");
        if (!std::filesystem::exists("data/client/universes/" + universeName))
            std::filesystem::create_directory("data/client/universes/" + universeName);
        if (!std::filesystem::exists("data/client/universes/" + universeName + "/worlds"))
            std::filesystem::create_directory("data/client/universes/" + universeName + "/worlds");
    }

    bool WorldManager::InsertWorld(const std::string& universeName, WorldType worldType, uint16_t worldSizeX, uint16_t worldSizeY)
    {
        CheckAndCreateFiles(universeName);

        std::mt19937 gen(std::random_device{}());
        world_id_t worldId = 0;
        do
        {
            std::uniform_int_distribution<world_id_t> worldIdDist(1, std::numeric_limits<world_id_t>::max());
            worldId = worldIdDist(gen);
        } while (worldsData.contains(worldId));

        std::string worldIdString{std::to_string(worldId)};
        std::filesystem::create_directory("data/server/universes/" + universeName + "/worlds/" + worldIdString);
        std::filesystem::create_directory("data/server/universes/" + universeName + "/worlds/" + worldIdString + "/regions");

        std::filesystem::create_directory("data/client/universes/" + universeName + "/worlds/" + worldIdString);
        std::filesystem::create_directory("data/client/universes/" + universeName + "/worlds/" + worldIdString + "/regions");

        universe_id_t universeId = UniverseManager::GetUniverseId(universeName);

        SQLite::Database* db = DatabaseManager::GetSharedDatabase();
        SQLite::Statement statement(*db, R"(
            INSERT INTO world(worldId, universeId, worldType, worldSeed, worldSizeX, worldSizeY)
            VALUES(?, ?, ?, ?, ?, ?)
        )");

        uint32_t universeSeed = UniverseManager::GetUniverseSeed(universeId);
        uint32_t worldSeed = universeSeed ^ (worldSizeX * 73856093) ^ (worldSizeY * 19349663) ^ (worldType * 14295323);

        statement.bind(1, worldId);
        statement.bind(2, universeId);
        statement.bind(3, worldType);
        statement.bind(4, worldSeed);
        statement.bind(5, worldSizeX);
        statement.bind(6, worldSizeY);

        return statement.exec() > 0;
    }

    bool WorldManager::DeleteWorld(const std::string& universeName, world_id_t worldId)
    {
        CheckAndCreateFiles(universeName);

        std::string worldIdString{std::to_string(worldId)};
        if (std::filesystem::exists("data/server/universes/" + universeName + "/worlds/" + worldIdString))
            std::filesystem::remove_all("data/server/universes/" + universeName + "/worlds/" + worldIdString);
        if (std::filesystem::exists("data/client/universes/" + universeName + "/worlds/" + worldIdString))
            std::filesystem::remove_all("data/client/universes/" + universeName + "/worlds/" + worldIdString);

        universe_id_t universeId = UniverseManager::GetUniverseId(universeName);

        SQLite::Database* db = DatabaseManager::GetSharedDatabase();
        SQLite::Statement statement(*db, "DELETE FROM world WHERE worldId = ? AND universeId = ?");
        statement.bind(1, worldId);
        statement.bind(2, universeId);

        return statement.exec() > 0;
    }

    void WorldManager::AddWorld(world_id_t worldId, WorldType worldType, uint32_t worldSeed, uint16_t worldSizeX, uint16_t worldSizeY)
    {
        worldsData.emplace(worldId, std::make_unique<WorldData>(worldId, worldType, worldSeed, worldSizeX, worldSizeY));
    }

    void WorldManager::RemoveWorld(world_id_t worldId)
    {
        if (worldsData.contains(worldId))
            worldsData.erase(worldId);
    }

    std::vector<WorldData*> WorldManager::GetWorlds()
    {
        std::vector<WorldData*> worlds{};
        for (const auto& world: worldsData)
        {
            worlds.push_back(world.second.get());
        }
        return worlds;
    }

    WorldData* WorldManager::GetWorldData(world_id_t worldId)
    {
        if (!worldsData.contains(worldId))
            return nullptr;
        return worldsData[worldId].get();
    }

    WorldGenerator* WorldManager::GetWorldGenerator(WorldType worldType)
    {
        if (!worldGenerators.contains(worldType))
            return nullptr;
        return worldGenerators[worldType].get();
    }

    void WorldManager::GenerateChunk(ChunkData* chunk, WorldType worldType)
    {
        WorldGenerator* worldGenerator = GetWorldGenerator(worldType);
        if (!worldGenerator)
            return;
        worldGenerator->Generate(chunk);
    }
}