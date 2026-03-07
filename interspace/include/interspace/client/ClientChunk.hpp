#pragma once

#include <memory>

#include "interspace/client/ClientTile.hpp"
#include "interspace/world/Chunk.hpp"

namespace Interspace::Client
{
    class ClientChunk : public Chunk<ClientTile>
    {
      public:
        Engine::Vec2<float> spritePosition{};

        std::shared_ptr<SDL_Texture> tileAtlas = nullptr;
        std::unique_ptr<Engine::Sprite> sprite = nullptr;

      public:
        explicit ClientChunk(const Engine::Vec2<uint16_t>& pos);

        void BeginTileUpdate();
        void EndTileUpdate();
        void UpdateTile(const Engine::Vec2<uint8_t>& tilePos, ClientTile* tile);
        void UpdateTiles(std::map<Engine::Vec2<uint8_t>, ClientTile*>& newTiles);
    };
}
