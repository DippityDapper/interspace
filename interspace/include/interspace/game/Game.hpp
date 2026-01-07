#pragma once

#include <memory>

#include "igneous/Scene.hpp"

#include "interspace/client/Client.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/world/WorldInterface.hpp"

namespace Interspace
{
    class Game: public Engine::Scene
    {
    public:
        static inline std::unique_ptr<Server::Server> server = nullptr;
        static inline std::unique_ptr<Client::Client> client = nullptr;

        static inline std::unique_ptr<WorldInterface> world = nullptr;

        static inline std::unique_ptr<NetInterface> serverNetInterface = nullptr;
        static inline std::unique_ptr<NetInterface> clientNetInterface = nullptr;

    private:
        void Init() override;
        void Update(float delta) override;
        void Render() override;
        void HandleEvents(SDL_Event& event) override;
        void Clean() override;

    public:
        static bool HostWorld(const std::string& worldName, int port, int peerCount, bool localOnly);
        static bool JoinWorld(const std::string& username, const std::string& ip, int port);
        static bool LoadWorld(const std::string& worldName, const std::string& username);
        static void Disconnect();
    };
}
