#pragma once

#include "interspace/server/world/ServerWorld.hpp"
#include "interspace/server/Server.hpp"
#include "igneous/scenes/Scene.hpp"

#include <memory>

namespace Interspace::Server
{
    class ServerUniverse : public Engine::Scene
    {
      private:
        Server* server = nullptr;

        uint32_t id = 0;
        uint32_t seed = 0;

        std::unordered_map<world_id_t, std::unique_ptr<ServerWorld>> worlds{};

      public:
        void Update(double delta) override;
        void InitUniverse(Server* _server);

        void CreateWorld(world_id_t worldId);
        ServerWorld* GetWorld(world_id_t worldId);
        void CreatePlayerEvent(const std::vector<uint8_t>& data, ENetPeer* from);
    };
}
