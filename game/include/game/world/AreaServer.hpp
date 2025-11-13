#pragma once

#include <random>
#include <map>

#include "dapper2d/Vec2.hpp"

#include "game/world/TileServer.hpp"

namespace Game
{
    class AreaServer
    {
    public:
        Engine::Vec2<uint16_t> position;
        std::map<Engine::Vec2<uint8_t>, TileServer*> tiles;

        uint32_t seed;
        std::mt19937 seedGen;

        bool generationComplete = false;

    public:
        AreaServer(Engine::Vec2<uint16_t> _position, uint32_t worldSeed);

        void Init();
        void Update(float delta);
        void Clean();

        bool Create();
        bool Load();
        bool Generate();
        std::vector<uint8_t> Serialize();
    };
}
