#pragma once

#include "igneous/engine/Vec2.hpp"
#include "interspace/game/Typedefs.hpp"

#include <cstdint>
#include <string>

namespace Interspace
{
    class Entity
    {
      public:
        entity_id_t id = 0;
        std::string name;
        Engine::Vec2<float> position{};
        uint16_t sight = 32;
    };
}