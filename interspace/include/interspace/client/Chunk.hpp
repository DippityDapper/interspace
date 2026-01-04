#pragma once

#include <map>
#include <memory>

#include "interspace/client/Tile.hpp"
#include "interspace/world/ChunkData.hpp"

namespace Interspace::Client
{
    class Chunk
    {
    public:
        ChunkData data{};

        Engine::Vec2<float> position{};

        std::map<Engine::Vec2<uint8_t>, Tile*> tiles{};
        std::shared_ptr<SDL_Texture> tileAtlas = nullptr;
        std::unique_ptr<Engine::Sprite> sprite = nullptr;

    public:
        explicit Chunk(const Engine::Vec2<uint16_t>& pos);

        void UpdateTile(const Engine::Vec2<uint8_t>& tilePos, Tile* tile);
        void UpdateTiles(std::map<Engine::Vec2<uint8_t>, Tile*>& newTiles);
    };
}
