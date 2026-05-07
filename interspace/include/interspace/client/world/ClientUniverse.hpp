#pragma once

#include "interspace/client/world/ClientWorld.hpp"
#include "igneous/scenes/Scene.hpp"
#include "interspace/client/Client.hpp"

#include <memory>

namespace Interspace::Client
{
    class ClientUniverse : public Engine::Scene
    {
      private:
        Client* client = nullptr;

        universe_id_t id = 0;
        uint32_t seed = 0;

        std::unique_ptr<ClientWorld> currentWorld = nullptr;

      public:
        void InitUniverse(Client* _client);
        void Update(double delta) override;
    };
}
