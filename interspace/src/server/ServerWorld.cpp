#include "interspace/server/ServerWorld.hpp"

#include "igneous/engine/Database.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/server/ServerOverworldGenerator.hpp"
#include "interspace/server/ServerTiles.hpp"

namespace Interspace::Server
{
    void ServerWorld::Init()
    {
        tileRegistry = std::make_unique<ServerTiles>();
        tileRegistry->Init();
    }

    void ServerWorld::InitWorld(Server* _server, const std::string& _worldName)
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

        worldGenerator = std::make_unique<ServerOverworldGenerator>(query.getColumn("worldSeed").getUInt());

        RegisterNetEvents();
        InitFactions();
    }

    void ServerWorld::Update(float delta)
    {
        if (serverTimer < serverClock)
            serverTimer += delta;
        else
        {
            serverTimer -= serverClock;
            server->CheckConnectionAttempts();
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
            worldGenerator->Generate();
            worldGenerator->HandleChunkRequests();
        }
    }

    void ServerWorld::Clean()
    {
        AutoSave();
    }

    void ServerWorld::AutoSave()
    {
    }
}
