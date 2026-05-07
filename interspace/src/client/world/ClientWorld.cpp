#include "interspace/client/world/ClientWorld.hpp"

#include "interspace/shared/datahelpers/WorldManager.hpp"

namespace Interspace::Client
{
    ClientWorld::ClientWorld(world_id_t worldId)
    {
        id = worldId;
    }
}