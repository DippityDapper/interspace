#pragma once

#include <cstdint>
#include <string>

#include "dapper2d/Vec2.hpp"

namespace Game
{
    class ColonistServer
    {
    public:
        Engine::Vec2<float> position{};
        uint32_t id = 0;
        std::string name{};

    public:
        ColonistServer() = default;
        ColonistServer(uint32_t _id, const std::string& _name, const Engine::Vec2<float>& _position);
    };
}
