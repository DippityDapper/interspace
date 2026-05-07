#pragma once

#include "interspace/shared/world/TileData.hpp"
#include "igneous/engine/Vec2.hpp"

#include <map>
#include <memory>

namespace Interspace
{
    struct ChunkData
    {
        Engine::Vec2<uint16_t> position{};
        uint64_t lastModified = 0;
        bool isModified = false;
        std::map<Engine::Vec2<uint8_t>, std::unique_ptr<TileData>> tiles{};
    };
}