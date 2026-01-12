#pragma once

#include <cstdint>
#include <string>

#include "igneous/engine/Vec2.hpp"

namespace Interspace
{
    struct EntityData
    {
        uint16_t id = 0;
        std::string name;
        Engine::Vec2<float> position{};
        uint16_t sight = 32;
    };
}
