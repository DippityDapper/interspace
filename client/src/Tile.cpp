#include "client/Tile.hpp"

#include "SDL3/SDL.h"

#include "dapper2d/Renderer.hpp"
#include "dapper2d/Sprite.hpp"

namespace Game
{
    int Tile::TILE_SIZE = 0;

    Tile::Tile(const Tile &tile)
    {
        sprite = std::make_unique<Engine::Sprite>(*tile.sprite);
    }

    Tile::Tile(const std::string& filePath)
    {
        sprite = std::make_unique<Engine::Sprite>(filePath);
    }

    Tile::Tile(const std::string& filePath, float w, float h, int x, int y)
    {
        sprite = std::make_unique<Engine::Sprite>(filePath, w, h, x, y);
    }

    void Tile::Render(int tilePositionX, int tilePositionY) const
    {
        if (!sprite)
            return;

        if (sprite->atlasW <= 0 || sprite->atlasH <= 0)
            return;
        Engine::Vec2<float> position{(float)tilePositionX * Tile::TILE_SIZE, (float)tilePositionY * Tile::TILE_SIZE};
        Engine::Renderer::BufferAdd(position, sprite.get());
    }
}