#pragma once

#include <map>
#include <memory>

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

        std::shared_ptr<SDL_Texture> cachedTexture = nullptr;

    public:
        Area(int _x, int _y);
        void Update(float delta);
        void Render();

        void GenerateTiles();
        void UpdateTile(Engine::Vec2<int> tilePosition, Tiles::Type tileType);
    };
}
