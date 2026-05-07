#pragma once

#include "interspace/shared/game/Typedefs.hpp"
#include "interspace/server/world/ServerChunk.hpp"

#include <cstdint>

namespace Interspace::Server
{
    class ServerWorld
    {
      private:
        world_id_t id = 0;
        std::map<Engine::Vec2<uint16_t>, std::unique_ptr<ServerChunk>> chunks{};

      public:
        ServerWorld(world_id_t worldId);
    };
}