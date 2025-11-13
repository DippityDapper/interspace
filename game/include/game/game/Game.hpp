#pragma once

#include <memory>

#include "dapper2d/Scene.hpp"

#include "game/network/Client.hpp"
#include "game/network/Server.hpp"
#include "game/world/World.hpp"

namespace Game
{
    class NetInterface;

    class Game: public Engine::Scene
    {
    private:
        static inline std::unique_ptr<Server> server = nullptr;
        static inline std::unique_ptr<Client> client = nullptr;
        static inline std::unique_ptr<World> world = nullptr;

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
