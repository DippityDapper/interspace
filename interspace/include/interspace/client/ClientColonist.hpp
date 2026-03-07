#pragma once

#include "interspace/client/ClientEntity.hpp"

#include <memory>

#include "igneous/resources/Sprite.hpp"
#include "interspace/world/Colonist.hpp"

namespace Interspace::Client
{
    class ClientColonist : public Colonist<ClientEntity>
    {
      public:
        std::unique_ptr<Engine::Sprite> sprite = nullptr;

      public:
        ClientColonist();

        void RenderName(const std::string& factionName);
    };
}
