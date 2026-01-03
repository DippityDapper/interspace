#pragma once

#include <map>
#include <memory>

#include "interspace/server/Tile.hpp"
#include "interspace/world/ChunkData.hpp"

namespace Interspace::Server
{
    class Chunk
    {
    public:
        ChunkData data{};
        std::map<uint8_t, std::unique_ptr<Tile>> tiles{};
    };
}
