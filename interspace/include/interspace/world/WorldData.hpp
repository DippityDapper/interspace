#pragma once

#include <cstdint>

namespace Interspace
{
    struct WorldData
    {
        uint16_t worldSizeX = 0;
        uint16_t worldSizeY = 0;

        const uint8_t TILE_SIZE = 32;
        const uint8_t CHUNK_SIZE = 64;
        const uint8_t REGION_SIZE = 32;
    };
}
