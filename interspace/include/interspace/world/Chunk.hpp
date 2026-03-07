#pragma once

#include "interspace/world/Tile.hpp"
#include "igneous/engine/Vec2.hpp"

#include <cstdint>
#include <map>

namespace Interspace
{
    template<typename T>
    concept ExtendsTile = std::is_base_of_v<Tile, T>;

    template<typename TileType>
    requires ExtendsTile<TileType>
    class Chunk
    {
      public:
        using tile_type = TileType;

        Engine::Vec2<uint16_t> position{};
        uint64_t lastModified = 0;
        bool isModified = false;
        std::map<Engine::Vec2<uint8_t>, TileType*> tiles{};
    };
}