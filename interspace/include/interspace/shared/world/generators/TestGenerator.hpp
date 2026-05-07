#pragma once
#include "WorldGenerator.hpp"

namespace Interspace
{
    class TestGenerator : public WorldGenerator
    {
      public:
        void Generate(ChunkData* chunk) override;
    };
}