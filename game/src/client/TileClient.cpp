#include "game/client/TileClient.hpp"

#include "dapper2d/Renderer.hpp"

#include "game/client/WorldClient.hpp"

namespace Game
{
    TileClient::TileClient(TileType _type, const std::string& texturePath, int w, int h, int x, int y)
    {
        type = _type;
        sprite = std::make_unique<Engine::Sprite>(texturePath, w, h, x, y);
    }

    void TileClient::LocalRender(const Engine::Vec2<float>& position)
    {
        Engine::Renderer::BufferAdd(position * WorldClient::TILE_SIZE, sprite.get());
    }
}
