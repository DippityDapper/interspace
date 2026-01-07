#pragma once

#include <cstdint>

#include "igneous/Vec2.hpp"

namespace Interspace
{
    struct ChunkData
    {
        Engine::Vec2<uint16_t> position{};
    };
}
