#include "interspace/server/world/ServerUniverse.hpp"

#include "interspace/shared/datahelpers/WorldManager.hpp"

namespace Interspace::Server
{
    void ServerUniverse::InitUniverse(Server* _server)
    {
        server = _server;

        for (const auto& world: WorldManager::GetWorlds())
        {
            worlds.emplace(world->id, std::make_unique<ServerWorld>(world->id));
        }
    }

    void ServerUniverse::Update(double delta)
    {
    }

    void ServerUniverse::CreateWorld(world_id_t worldId)
    {
        worlds.emplace(worldId, std::make_unique<ServerWorld>(worldId));
    }

    ServerWorld* ServerUniverse::GetWorld(world_id_t worldId)
    {
        if (!worlds.contains(worldId))
            return nullptr;
        return worlds[worldId].get();
    }
}
