#include "interspace/client/ClientChunk.hpp"

#include "igneous/rendering/Renderer.hpp"
#include "igneous/resources/ResourceManager.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/world/WorldData.hpp"

namespace Interspace::Client
{
    ClientChunk::ClientChunk(const Engine::Vec2<uint16_t>& pos)
    {
        WorldData* worldData = Game::clientWorld->worldData.get();

        position = pos;
        spritePosition = (Engine::Vec2<float>) pos * worldData->CHUNK_SIZE * worldData->TILE_SIZE;

        tileAtlas = Engine::ResourceManager::CreateTexture(
                SDL_PIXELFORMAT_RGBA8888,
                SDL_TEXTUREACCESS_TARGET,
                worldData->CHUNK_SIZE * worldData->TILE_SIZE,
                worldData->CHUNK_SIZE * worldData->TILE_SIZE);

        sprite = std::make_unique<Engine::Sprite>(spritePosition, tileAtlas);
        sprite->centered = false;
        sprite->SetZIndex(-1);
    }

    void ClientChunk::BeginTileUpdate()
    {
        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), tileAtlas.get());
    }

    void ClientChunk::EndTileUpdate()
    {
        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), nullptr);
    }

    void ClientChunk::UpdateTile(const Engine::Vec2<uint8_t>& tilePos, ClientTile* tile)
    {
        WorldData* worldData = Game::clientWorld->worldData.get();
        Engine::Vec2<float> tileLocalPos{(float) tilePos.x * worldData->TILE_SIZE, (float) tilePos.y * worldData->TILE_SIZE};

        Engine::Renderer::BufferAdd(tileLocalPos, tile->sprite.get());
    }

    void ClientChunk::UpdateTiles(std::map<Engine::Vec2<uint8_t>, ClientTile*>& newTiles)
    {
        for (const auto& newTile: newTiles)
        {
            WorldData* worldData = Game::clientWorld->worldData.get();
            ClientTile* tile = newTile.second;
            Engine::Vec2<uint8_t> tilePos = newTile.first;
            Engine::Vec2<float> tileLocalPos{(float) tilePos.x * worldData->TILE_SIZE, (float) tilePos.y * worldData->TILE_SIZE};

            Engine::Renderer::BufferAdd(tileLocalPos, tile->sprite.get());
            Engine::Renderer::BufferAdd(sprite->position, sprite.get(), nullptr);
        }
    }
}
