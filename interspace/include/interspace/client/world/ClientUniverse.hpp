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
        universe_id_t id = 0;
        uint32_t seed = 0;

        std::unique_ptr<ClientWorld> currentWorld = nullptr;

      public:
        void OnCreated() override;
        void Update(double delta) override;

        void SendConnectionRequest();
        void OnServerRemotePacket(const std::vector<uint8_t>& data);
        void OnConnectionAccepted(const std::vector<uint8_t>& data);
        void OnDisconnectAcknowledged(const std::vector<uint8_t>& data);
        void OnServerDisconnected(const std::vector<uint8_t>& data);
    };
}
