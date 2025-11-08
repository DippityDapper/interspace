#pragma once

#include <cstdint>
#include <string>

namespace Server
{
    class WorldData
    {
    private:
        std::string name{""};

    public:
        //std::map<Engine::Vec2<int>, std::unique_ptr<Game::Area>> areas;

        static uint32_t worldSeed;
        static int WORLD_SIZE_X;
        static int WORLD_SIZE_Y;
        static int REGION_SIZE;

        //std::map<int, std::unique_ptr<ClientEntity>> entities{};

    public:
        WorldData(const std::string& _name);
    };
}
