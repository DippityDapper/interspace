#include "interspace/server/World.hpp"

#include "igneous/engine/Database.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/server/Tiles.hpp"

namespace Interspace::Server
{
    void World::Init()
    {
        Tiles::Init();
    }

    void World::InitWorld(Server* _server, const std::string& _worldName)
    {
        server = _server;
        worldName = _worldName;
        DBHelper::LoadWorld(worldName);

        SQLite::Statement query(*DBHelper::worldsDb, "SELECT worldSizeX, worldSizeY, worldSeed FROM world WHERE worldId = ?");
        query.bind(1, worldName);
        query.executeStep();

        worldData = std::make_unique<WorldData>();
        worldData->worldSizeX = query.getColumn("worldSizeX").getUInt();
        worldData->worldSizeY = query.getColumn("worldSizeY").getUInt();

        worldGenerator = std::make_unique<WorldGenerator>(query.getColumn("worldSeed").getUInt());

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
            worldGenerator->GenerateNaturalChunks();
            worldGenerator->HandleChunkRequests();
        }
    }

    void World::UI()
    {
    }

    void World::Clean()
    {
        AutoSave();
    }

    void World::AutoSave()
    {
    }
}
