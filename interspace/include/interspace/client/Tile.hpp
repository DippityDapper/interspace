#pragma once

#include "igneous/Sprite.hpp"
#include "interspace/world/TileData.hpp"

namespace Interspace::Client
{
    class Tile
    {
    public:
        TileData data{};

        std::unique_ptr<Engine::Sprite> sprite = nullptr;
    };
}
