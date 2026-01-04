#include "interspace/client/Chunk.hpp"

#include "igneous/Renderer.hpp"
#include "igneous/ResourceLoader.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/world/WorldData.hpp"

namespace Interspace::Client
{
    Chunk::Chunk(const Engine::Vec2<uint16_t>& pos)
    {
        WorldData* worldData = Game::world->clientWorld->worldData.get();

        data.position = pos;
        position = (Engine::Vec2<float>)pos * worldData->CHUNK_SIZE * worldData->TILE_SIZE;

        tileAtlas = Engine::ResourceLoader::CreateTexture(
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            worldData->CHUNK_SIZE * worldData->TILE_SIZE,
            worldData->CHUNK_SIZE * worldData->TILE_SIZE
        );

        sprite = std::make_unique<Engine::Sprite>(position, tileAtlas);
        sprite->centered = false;
        sprite->SetZIndex(-1);
    }

    void Chunk::UpdateTile(const Engine::Vec2<uint8_t>& tilePos, Tile* tile)
    {
        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), tileAtlas.get());

        WorldData* worldData = Game::world->clientWorld->worldData.get();
        Engine::Vec2<float> tileLocalPos{(float)tilePos.x * worldData->TILE_SIZE, (float)tilePos.y * worldData->TILE_SIZE};

        auto tileTexture = Engine::ResourceLoader::LoadTexture(tile->texturePath);
        std::unique_ptr<Engine::Sprite> tileSprite = std::make_unique<Engine::Sprite>(
            tileLocalPos,
            tileTexture,
            tile->atlasWidth,
            tile->atlasHeight,
            tile->atlasX,
            tile->atlasY);

        tileSprite->centered = false;

        Engine::Renderer::BufferAdd(tileSprite->position, tileSprite.get());

        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), nullptr);
    }

    void Chunk::UpdateTiles(std::map<Engine::Vec2<uint8_t>, Tile*>& newTiles)
    {
        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), tileAtlas.get());

        for (const auto& newTile : newTiles)
        {
            WorldData* worldData = Game::world->clientWorld->worldData.get();
            Tile* tile = newTile.second;
            Engine::Vec2<uint8_t> tilePos = newTile.first;
            Engine::Vec2<float> tileGlobalPos{(float)tilePos.x * worldData->TILE_SIZE, (float)tilePos.y * worldData->TILE_SIZE};

            auto tileTexture = Engine::ResourceLoader::LoadTexture(tile->texturePath);
            std::unique_ptr<Engine::Sprite> tileSprite = std::make_unique<Engine::Sprite>(tileGlobalPos, tileTexture);
            Engine::Renderer::BufferAdd(sprite->position, sprite.get(), nullptr);
        }

        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), nullptr);
    }
}
