#include "client/Area.hpp"

#include <random>

#include "SDL3/SDL.h"

#include "dapper2d/Window.hpp"
#include "dapper2d/Camera.hpp"
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
    }

    void Area::GenerateTiles()
    {
        std::mt19937 gen(areaSeed);

        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), cachedTexture.get());

        for (int y = 0; y < AREA_SIZE; ++y)
        {
            for (int x = 0; x < AREA_SIZE; ++x)
            {
                std::uniform_int_distribution<> flowerChance(0, 9);
                bool isFlower = flowerChance(gen) == 0;

                Tiles::Type tileType = Tiles::GRASS_1;
                Engine::Vec2<int> localPosition{x, y};
                if (localPosition.x == 0 || localPosition.x == AREA_SIZE-1 || localPosition.y == 0 || localPosition.y == AREA_SIZE-1)
                {
                    tileType = Tiles::STONE_PATH;
                }
                else if (isFlower)
                {
                    std::uniform_int_distribution<> flowerTileDistribution((int)Tiles::FLOWER_1, (int)Tiles::FLOWER_16);
                    tileType = (Tiles::Type)flowerTileDistribution(gen);
                }
                else
                {
                    std::uniform_int_distribution<> grassTileDistribution((int)Tiles::GRASS_1, (int)Tiles::GRASS_16);
                    tileType = (Tiles::Type)grassTileDistribution(gen);
                }

                Tile* tile = Tiles::GetTile(tileType);
                tiles[localPosition] = tile;



                Engine::Vec2<float> globalTilePosition = (Engine::Vec2<float>)localPosition * Tile::TILE_SIZE;
                Engine::Renderer::BufferAddNoOffset(globalTilePosition, tile->sprite);
            }
        }

        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), nullptr);
    }

    void Area::Update(float delta)
    {
    }

    void Area::UpdateTile(Engine::Vec2<int> tilePosition, Tiles::Type tileType)
    {
        Tile* tile = Tiles::GetTile(tileType);
        tiles[tilePosition] = tile;

        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), cachedTexture.get());

        Engine::Vec2<float> globalTilePosition = (Engine::Vec2<float>)tilePosition * Tile::TILE_SIZE;
        Engine::Renderer::BufferAddNoOffset(globalTilePosition, tile->sprite);

        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), nullptr);
    }

    void Area::Render()
    {
        if (!cachedTexture)
            return;
        Engine::Vec2<float> areaPosition = (Engine::Vec2<float>)position * Area::AREA_SIZE * Tile::TILE_SIZE;
        Engine::Renderer::BufferAdd(areaPosition, cachedTexture.get());
    }
}
