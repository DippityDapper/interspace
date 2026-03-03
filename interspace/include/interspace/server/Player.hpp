#pragma once

#include "igneous/engine/Vec2.hpp"

namespace Interspace::Server
{
    class Player
    {
      public:
        Engine::Vec2<float> position{0, 0};
    };
}