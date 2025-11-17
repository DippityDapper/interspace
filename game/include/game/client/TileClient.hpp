#pragma once

#include "dapper2d/Sprite.hpp"
#include "dapper2d/Vec2.hpp"

#include "game/world/TileType.hpp"

namespace Game
{
    class TileClient
    {
    public:
        TileType type = GRASS_1;
        std::unique_ptr<Engine::Sprite> sprite = nullptr;

    public:
        TileClient(TileType _type, const std::string& texturePath, int w, int h, int x, int y);

        void LocalRender(const Engine::Vec2<float>&);
    };
}
