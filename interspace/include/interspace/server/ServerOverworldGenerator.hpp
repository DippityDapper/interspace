#pragma once
#include "interspace/server/ServerWorldGenerator.hpp"

namespace Interspace::Server
{
    class ServerOverworldGenerator : public ServerWorldGenerator
    {
      public:
        ServerOverworldGenerator(uint32_t seed) : ServerWorldGenerator(seed)
        {
        }

        void GenerateChunk(ServerChunk* chunk) override;
    };
}
