#pragma once

#include "SDL3/SDL.h"

#include "common/vector/Vec2.h"

#include "client/camera/camera.h"
#include "client/engine/Renderer.h"

namespace Engine
{
    class Tile
    {
    public:
        Vec2<int> gridPosition{0,0};
        Sprite* sprite = nullptr;

    public:
        Tile(Vec2<int> _gridPosition, std::string& texturePath);
        Tile(Vec2<int> _gridPosition, std::string& texturePath, float w, float h, int x, int y);
        ~Tile();
        Tile(const Tile&) = delete;
        Tile& operator=(const Tile&) = delete;

        void Render(int offsetX, int offsetY) const;
    };
}
