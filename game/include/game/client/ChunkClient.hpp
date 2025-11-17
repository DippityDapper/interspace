#pragma once

#include <map>
#include <memory>
#include <vector>

#include "game/client/TileClient.hpp"
#include "SDL3/SDL_render.h"

#include "dapper2d/Vec2.hpp"

namespace Game
{
    class ChunkClient
    {
    private:
        Engine::Vec2<uint16_t> position;
        std::map<Engine::Vec2<uint8_t>, TileClient*> tiles;
        std::vector<uint8_t> queuedTileData{};

        float fogAlpha = 0.0f;
        float fadeInSpeed = 0.3f;

    public:
        std::shared_ptr<SDL_Texture> bakedTexture = nullptr;
        bool generationComplete = false;

    public:
        ChunkClient(const Engine::Vec2<uint16_t>& _position, const std::vector<uint8_t>& tileData);

        void Init();
        void Update(float delta);
        void Clean();
        bool Create();

        void BakeSprite() const;
        void Render();
    };
}
