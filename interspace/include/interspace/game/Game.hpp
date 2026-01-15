#pragma once

#include <memory>

#include "igneous/scenes/Scene.hpp"

#include "interspace/client/Client.hpp"
#include "interspace/client/World.hpp"
#include "interspace/menus/MainMenu.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/server/World.hpp"

namespace Interspace
{
    class Game: public Engine::Scene
    {
    private:
        static inline bool pendingDisconnect = false;

    public:
        static inline std::unique_ptr<Server::Server> server = nullptr;
        static inline std::unique_ptr<Client::Client> client = nullptr;

        static inline Server::World* serverWorld = nullptr;
        static inline Client::World* clientWorld = nullptr;

        static inline std::unique_ptr<Engine::NetworkInterface> serverNetInterface = nullptr;
        static inline std::unique_ptr<Engine::NetworkInterface> clientNetInterface = nullptr;

        static inline MainMenu* mainMenu = nullptr;

    private:
        void Init() override;
        void Update(float delta) override;
        void Render() override;
        void HandleEvents(Engine::InputLayer& layer) override;
        void Clean() override;

    public:
        static bool HostWorld(const std::string& worldName, int port, int peerCount, bool localOnly);
        static bool JoinWorld(const std::string& username, const std::string& ip, int port);
        static bool LoadWorld(const std::string& worldName, const std::string& username);
        static void Disconnect();
    };
}
