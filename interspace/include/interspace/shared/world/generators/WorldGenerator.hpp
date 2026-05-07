#pragma once

#include "interspace/shared/world/ChunkData.hpp"

namespace Interspace
{
    class WorldGenerator
    {
      public:
        virtual ~WorldGenerator() = default;
        virtual void Generate(ChunkData* chunk) = 0;
    };
}