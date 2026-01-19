#pragma once

#include "interspace/server/Entity.hpp"
#include "interspace/world/ColonistData.hpp"

namespace Interspace::Server
{
    class Colonist
    {
      public:
        EntityData entityData{};
        ColonistData colonistData{};
    };
}
