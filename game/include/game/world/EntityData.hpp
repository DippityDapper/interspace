#pragma once

#include <string>
#include <cstdint>

#include "igneous/Vec2.hpp"

namespace Game
{
    struct EntityData
    {
        uint16_t id = 0;
        std::string name;
        Engine::Vec2<float> position{};
    };
}
