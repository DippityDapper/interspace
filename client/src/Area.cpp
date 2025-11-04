#include "client/Area.hpp"

#include "SDL3/SDL.h"

#include "dapper2d/Window.hpp"
#include "dapper2d/Renderer.hpp"
#include "dapper2d/Sprite.hpp"
#include <dapper2d/ResourceLoader.hpp>

#include "client/Tile.hpp"
#include "client/World.hpp"

namespace Game
{
    int Area::AREA_SIZE = 0;

    Area::Area(int _x, int _y)
    {
        position.x = _x;
        position.y = _y;

        areaSeed = World::worldSeed ^ (position.x * 73856093) ^ (position.y * 19349663);

        cachedTexture = Engine::ResourceLoader::CreateTexture(
                SDL_PIXELFORMAT_RGBA8888,
                SDL_TEXTUREACCESS_TARGET,
                AREA_SIZE * Tile::TILE_SIZE,
                AREA_SIZE * Tile::TILE_SIZE
        );

        fogTexture = Engine::ResourceLoader::LoadTexture("tiles/fog.png");
    }

    void Area::GenerateTiles()
    {
        seedGen.seed(areaSeed);
        tiles.clear();
        cachedTexture = Engine::ResourceLoader::CreateTexture(
                SDL_PIXELFORMAT_RGBA8888,
                SDL_TEXTUREACCESS_TARGET,
                AREA_SIZE * Tile::TILE_SIZE,
                AREA_SIZE * Tile::TILE_SIZE
        );
    }

    void Area::Update(float delta)
    {
        if (tiles.size() < AREA_SIZE * AREA_SIZE)
        {
            SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), cachedTexture.get());

            int count = 0;

            while (tiles.size() < AREA_SIZE * AREA_SIZE && count < maxTilePerUpdate)
            {
                int x = tiles.size() % AREA_SIZE;
                int y = tiles.size() / AREA_SIZE;

                std::uniform_int_distribution<> flowerChance(0, 9);
                bool isFlower = flowerChance(seedGen) == 0;

                Tiles::Type tileType = Tiles::GRASS_1;
                Engine::Vec2<int> localPosition{x, y};
                if (localPosition.x == 0 || localPosition.x == AREA_SIZE-1 || localPosition.y == 0 || localPosition.y == AREA_SIZE-1)
                {
                    tileType = Tiles::STONE_PATH;
                }
                else if (isFlower)
                {
                    std::uniform_int_distribution<> flowerTileDistribution((int)Tiles::FLOWER_1, (int)Tiles::FLOWER_16);
                    tileType = (Tiles::Type)flowerTileDistribution(seedGen);
                }
                else
                {
                    std::uniform_int_distribution<> grassTileDistribution((int)Tiles::GRASS_1, (int)Tiles::GRASS_16);
                    tileType = (Tiles::Type)grassTileDistribution(seedGen);
                }

                Tile* tile = Tiles::GetTile(tileType);
                tiles[localPosition] = tile;

                Engine::Vec2<float> scaledTilePosition = (Engine::Vec2<float>)localPosition * Tile::TILE_SIZE;
                Engine::Renderer::BufferAdd(scaledTilePosition, tile->sprite);

                count++;
            }

            SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), nullptr);
        }
        else if (!fadingOut)
        {
            fadingOut = true;
        }

        if (fadingOut && fogAlpha > 0.0f)
        {
            fogAlpha -= fogFadeSpeed * delta;
            if (fogAlpha < 0.0f)
                fogAlpha = 0.0f;
        }
    }

    void Area::UpdateTile(Engine::Vec2<int> tilePosition, Tiles::Type tileType)
    {
        if (tiles.size() < AREA_SIZE * AREA_SIZE)
            return;

        Tile* tile = Tiles::GetTile(tileType);
        tiles[tilePosition] = tile;

        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), cachedTexture.get());

        Engine::Vec2<float> globalTilePosition = (Engine::Vec2<float>)tilePosition * Tile::TILE_SIZE;
        Engine::Renderer::BufferAdd(globalTilePosition, tile->sprite);

        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), nullptr);
    }

    void Area::Render()
    {
        if (!cachedTexture)
            return;

        Engine::Vec2<float> areaPosition = (Engine::Vec2<float>)position * Area::AREA_SIZE * Tile::TILE_SIZE;
        Engine::Renderer::BufferAdd(areaPosition, cachedTexture.get(), nullptr);

        if (fogTexture && fogAlpha > 0.0f)
        {
            SDL_SetTextureAlphaMod(fogTexture.get(), static_cast<Uint8>(fogAlpha * 255));

            int areaPixels = Area::AREA_SIZE * Tile::TILE_SIZE;
            int fogSize = 512;

            for (int y = 0; y < areaPixels; y += fogSize)
            {
                for (int x = 0; x < areaPixels; x += fogSize)
                {
                    Engine::Vec2<float> fogPos = areaPosition + Engine::Vec2<float>{(float)x, (float)y};
                    Engine::Renderer::BufferAdd(fogPos, fogTexture.get(), nullptr);
                }
            }
        }
    }
}
