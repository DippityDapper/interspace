#pragma once

#include "WorldTypes.hpp"
#include "interspace/shared/game/Typedefs.hpp"

namespace Interspace
{
    struct WorldData
    {
        world_id_t id = 0;
        uint32_t seed = 0;
        WorldType type = Test;

        uint16_t worldSizeX = 0;
        uint16_t worldSizeY = 0;

        static constexpr uint8_t TILE_SIZE = 32;
        static constexpr uint8_t CHUNK_SIZE = 64;
        static constexpr uint8_t REGION_SIZE = 32;

        WorldData(world_id_t _id, WorldType _type, uint32_t _seed, uint16_t _worldSizeX, uint16_t _worldSizeY)
        {
            id = _id;
            type = _type;
            worldSizeX = _worldSizeX;
            worldSizeY = _worldSizeY;
        }
    };
}