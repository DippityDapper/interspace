#pragma once

#include <map>
#include <memory>
#include <vector>

#include "dapper2d/Vec2.hpp"

#include "game/client/TileClient.hpp"

namespace Game
{
    class AreaClient
    {
    private:
        std::vector<uint8_t> queuedTileData{};

    public:
        Engine::Vec2<uint16_t> position;
        std::shared_ptr<SDL_Texture> bakedTexture = nullptr;

        std::map<Engine::Vec2<uint8_t>, TileClient*> tiles{};

        bool generationComplete = false;

    public:
        explicit AreaClient(const Engine::Vec2<uint16_t>& _position, const std::vector<uint8_t>& tileData);
        void Create();
        void BakeSprite();
        void Update(float delta);
        void Render();
    };
}
