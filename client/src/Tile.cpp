#include "client/Tile.hpp"

#include "SDL3/SDL.h"

#include "dapper2d/Renderer.hpp"
#include "dapper2d/Sprite.hpp"

namespace Game
{
    const int Tile::TILE_SIZE = 32;

    Tile::Tile(Engine::Vec2<int> _gridPosition, std::string& texturePath)
    {
        gridPosition.x = _gridPosition.x;
        gridPosition.y = _gridPosition.y;
        sprite = new Engine::Sprite(texturePath);
    }

    Tile::Tile(Engine::Vec2<int> _gridPosition, std::string &texturePath, float w, float h, int x, int y)
    {
        gridPosition.x = _gridPosition.x;
        gridPosition.y = _gridPosition.y;
        sprite = new Engine::Sprite(texturePath, Tile::TILE_SIZE, Tile::TILE_SIZE, x, y);
    }

    Tile::~Tile()
    {
        delete sprite;
    }

    void Tile::Render(int offsetX, int offsetY) const
    {
        if (!sprite)
            return;

        if (sprite->tileW <= 0 || sprite->tileH <= 0)
            return;

        Engine::Vec2<float> position{(float)(gridPosition.x + offsetX) * sprite->tileW, (float)(gridPosition.y + offsetY) * sprite->tileH};
        Engine::Renderer::BufferAdd(position, sprite);
    }
}