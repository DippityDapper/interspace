#pragma once

#include <map>
#include <memory>
#include <set>

#include "interspace/server/Tile.hpp"
#include "interspace/world/ChunkData.hpp"

namespace Interspace::Server
{
    class Chunk
    {
    public:
        ChunkData data{};
        std::map<Engine::Vec2<uint8_t>, Tile*> tiles{};
        std::set<uint16_t> seenBy{};
    };
}
