#pragma once

#include "ClientChunk.hpp"
#include "interspace/shared/game/Typedefs.hpp"
#include "interspace/shared/world/ChunkData.hpp"

namespace Interspace::Client
{
    class ClientWorld
    {
      public:
        world_id_t id = 0;
        std::map<Engine::Vec2<uint16_t>, std::unique_ptr<ClientChunk>> chunks{};

      public:
        ClientWorld(world_id_t worldId);
    };
}