#pragma once

#include <map>
#include <memory>

#include "game/client/Tile.hpp"
#include "game/world/ChunkData.hpp"

namespace Game::Client
{
    class Chunk
    {
    public:
        ChunkData data{};
        std::map<uint8_t, std::unique_ptr<Tile>> tiles{};
    };
}
