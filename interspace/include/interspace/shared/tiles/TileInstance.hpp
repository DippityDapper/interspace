#pragma once
#include <cstdint>

namespace Interspace
{
    struct TileDefinition
    {
        uint16_t definitionId;
        uint8_t stateIndex;
        uint8_t flags;
    };
}
