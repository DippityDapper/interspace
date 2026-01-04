#include "interspace/server/World.hpp"

#include <ranges>

#include "SDL3/SDL_log.h"

#include "igneous/CFGParser.hpp"
#include "igneous/Database.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/server/Tiles.hpp"

namespace Interspace::Server
{
    World::World(Server* _server, const std::string& _worldName)
    {
        server = _server;
        worldName = _worldName;
    }

    void World::Init()
    {
        Tiles::Init();
        DBHelper::UpdateWorldLastPlayed(worldName);

        SQLite::Database* db = DBHelper::db.get();
        auto worldResult = Engine::Database::Query(db, "SELECT * FROM world WHERE worldId = '" + worldName + "'");

        if (worldResult.empty() || worldResult.size() > 1)
        {
            SDL_Log("[Server] Error with world. Multiple worlds of name %s found. Using the first found world.", worldName.c_str());
        }

        seed = std::stoul(worldResult[0]["worldSeed"]);

        worldData = std::make_unique<WorldData>();
        worldData->worldSizeX = std::stoul(worldResult[0]["worldSizeX"]);
        worldData->worldSizeY = std::stoul(worldResult[0]["worldSizeY"]);

        RegisterNetEvents();
        InitFactions();
    }

    void World::Update(float delta)
    {
        if (serverTimer < serverClock)
            serverTimer += delta;
        else
        {
            serverTimer -= serverClock;

            GenerateChunks();

            BroadcastColonistPositionData();
        }

        if (autosaveTimer < autosaveClock)
            autosaveTimer += delta;
        else
        {
            autosaveTimer -= autosaveClock;
            AutoSave();
        }

        if (chunkGenerationTimer < chunkGenerationClock)
            chunkGenerationTimer += delta;
        else
        {
            chunkGenerationTimer -= chunkGenerationClock;
            BeginChunkGeneration();
            BroadcastChunksData();
        }
    }

    void World::Render()
    {
    }

    void World::HandleEvents(SDL_Event& event)
    {
    }

    void World::Clean()
    {
        AutoSave();
    }

    void World::AutoSave()
    {
        SQLite::Database* db = DBHelper::db.get();

        try
        {
            SQLite::Statement stmt(*db,
                "UPDATE colonist SET colonistX = ?, colonistY = ? "
                "WHERE colonistId = ? AND worldId = ?");

            db->exec("BEGIN TRANSACTION");

            for (const auto& faction : factions | std::views::values)
            {
                for (const auto& colonist : faction->colonists | std::views::values)
                {
                    stmt.bind(1, colonist->entityData.position.x);
                    stmt.bind(2, colonist->entityData.position.y);
                    stmt.bind(3, colonist->entityData.id);
                    stmt.bind(4, worldName);

                    stmt.exec();
                    stmt.reset();
                }
            }

            db->exec("COMMIT");
        }
        catch (const std::exception& e)
        {
            db->exec("ROLLBACK");
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "AutoSave failed: %s", e.what());
        }
    }
}
