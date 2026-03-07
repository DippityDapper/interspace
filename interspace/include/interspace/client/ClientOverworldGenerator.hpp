#pragma once

#include "interspace/client/ClientWorldGenerator.hpp"

namespace Interspace::Client
{
    class ClientOverworldGenerator : public ClientWorldGenerator
    {
      public:
        ClientOverworldGenerator(uint32_t _seed) : ClientWorldGenerator(_seed)
        {
        }
        void GenerateChunk(ClientChunk* chunk) override;
    };
}