#pragma once

#include <memory>
#include <string>

#include "igneous/scenes/Scene.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/server/Server.hpp"

namespace Interspace
{
    class NetworkManager : public Engine::Scene
    {
      public:
        static constexpr int PORT = 27030;

        static inline std::unique_ptr<Server::Server> server = nullptr;
        static inline std::unique_ptr<Client::Client> client = nullptr;

        void Update(double delta) override;

        static void Clean();

        static void CreateServer(int maxClients, bool localOnly = false);

        static void CreateClient(const std::string& ip);

        static void CreateSteamServer();

        static void CreateSteamClient(const std::string& ip);

        static void CreateLocalClientServer();

        static void CreateRemoteClientServer(int maxClients, bool localOnly = false);

        static void CreateSteamClientServer();
    };
}
