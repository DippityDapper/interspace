#pragma once

#include <map>
#include <memory>

#include "game/server/Tile.hpp"
#include "game/world/ChunkData.hpp"

namespace Game::Server
{
    class Chunk
    {
    public:
        ChunkData data{};
        std::map<uint8_t, std::unique_ptr<Tile>> tiles{};
    };
}
