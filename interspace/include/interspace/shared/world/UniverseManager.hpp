#pragma once

#include "interspace/client/world/ClientUniverse.hpp"
#include "interspace/server/world/ServerUniverse.hpp"

namespace Interspace
{
    class UniverseManager : public Engine::Scene
    {
      public:
        static inline Server::ServerUniverse* serverUniverse = nullptr;
        static inline Client::ClientUniverse* clientUniverse = nullptr;

      public:
        void OnCreated() override;
    };
}