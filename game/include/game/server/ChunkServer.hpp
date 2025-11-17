#pragma once

#include <map>
#include <random>

#include "game/server/TileServer.hpp"
#include "dapper2d/Vec2.hpp"

namespace Game
{
    class ChunkServer
    {
    public:
        Engine::Vec2<uint16_t> position;
        std::map<Engine::Vec2<uint8_t>, TileServer*> tiles;

        uint32_t seed = 0;
        std::mt19937 seedGen{};

        bool generationComplete = false;

    public:
        ChunkServer(Engine::Vec2<uint16_t> _position, uint32_t worldSeed);

        void Init();
        void Update(float delta);
        void Clean();
        bool Create();

        bool Load();
        bool Generate();

        std::vector<uint8_t> Serialize() const;
    };
}
