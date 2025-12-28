#pragma once

#include "igneous/Sprite.hpp"
#include "game/world/TileData.hpp"

namespace Game::Client
{
    class Tile
    {
    public:
        TileData data{};
        std::unique_ptr<Engine::Sprite> sprite = nullptr;
    };
}
