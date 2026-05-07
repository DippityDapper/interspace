#pragma once

#include <memory>

#include "igneous/scenes/Scene.hpp"

#include "interspace/client/world/ClientUniverse.hpp"
#include "interspace/client/menus/MainMenu.hpp"
#include "interspace/server/world/ServerUniverse.hpp"

namespace Interspace
{
    class Game : public Engine::Scene
    {
      public:
        static inline Server::ServerUniverse* serverUniverse = nullptr;
        static inline Client::ClientUniverse* clientUniverse = nullptr;
        static inline MainMenu* mainMenu = nullptr;

      private:
        void OnCreated() override;
        void Update(double delta) override;

        void InitSystems();

      public:
        static bool HostUniverse(const std::string& universeName, int peerCount, bool localOnly);
        static bool JoinUniverse(const std::string& ip);
        static bool LoadUniverse(const std::string& universeName);
    };
}
