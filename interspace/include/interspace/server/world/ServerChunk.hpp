#pragma once
#include "interspace/shared/world/ChunkData.hpp"

#include <memory>

namespace Interspace::Server
{
    struct ServerChunk
    {
        std::unique_ptr<ChunkData> chunkData = nullptr;
    };
}