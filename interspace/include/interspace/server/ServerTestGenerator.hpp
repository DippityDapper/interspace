#pragma once

#include "interspace/server/ServerWorldGenerator.hpp"

namespace Interspace::Server
{
    class ServerTestGenerator : public ServerWorldGenerator
    {
      public:
        ServerTestGenerator(uint32_t seed) : ServerWorldGenerator(seed)
        {
        }

        void GenerateChunk(ServerChunk* chunk) override;
    };
}