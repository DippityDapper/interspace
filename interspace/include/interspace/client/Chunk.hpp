#pragma once

#include <map>
#include <memory>

#include "interspace/client/Tile.hpp"
#include "interspace/world/ChunkData.hpp"

namespace Interspace::Client
{
    class Chunk
    {
    public:
        ChunkData data{};
        std::map<uint8_t, std::unique_ptr<Tile>> tiles{};
    };
}
