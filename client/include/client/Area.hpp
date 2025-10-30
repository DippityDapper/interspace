#pragma once

#include <map>
#include <memory>
#include <random>

#include "dapper2d/Vec2.hpp"
#include "client/Tiles.hpp"

struct SDL_Texture;

namespace Game
{
    class Tile;

    class Area
    {
    public:
        static int AREA_SIZE;
        uint32_t areaSeed = 0;

        std::map<Engine::Vec2<int>, Tile*> tiles{};
        Engine::Vec2<int> position{0,0};

        int maxTilePerUpdate = 16;
        std::mt19937 seedGen;
        std::shared_ptr<SDL_Texture> cachedTexture = nullptr;

        std::shared_ptr<SDL_Texture> fogTexture = nullptr;
        float fogAlpha = 1.0f;
        bool fadingOut = false;
        float fogFadeSpeed = 1.5f;

    public:
        Area(int _x, int _y);
        void Update(float delta);
        void Render();

        void GenerateTiles();
        void UpdateTile(Engine::Vec2<int> tilePosition, Tiles::Type tileType);
    };
}
