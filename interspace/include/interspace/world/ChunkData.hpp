#pragma once

#include <cstdint>

#include "igneous/engine/Vec2.hpp"

namespace Interspace
{
    struct ChunkData
    {
        Engine::Vec2<uint16_t> position{};
        uint64_t lastModified = 0;
        bool isModified = false;
    };
}
