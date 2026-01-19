#pragma once

#include <memory>

#include "igneous/resources/Sprite.hpp"

#include "interspace/client/Entity.hpp"
#include "interspace/world/ColonistData.hpp"

namespace Interspace::Client
{
    class Colonist
    {
      public:
        EntityData entityData{};
        ColonistData colonistData{};

        std::unique_ptr<Engine::Sprite> sprite = nullptr;

      public:
        Colonist();

        void Update(float delta);
        void RenderName(const std::string& factionName);
    };
}
