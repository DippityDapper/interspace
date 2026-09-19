#pragma once

#include "igneous/scenes/Scene.hpp"
#include "interspace/client/Client.hpp"

namespace Interspace::Client
{
    class ClientUniverse : public Engine::Scene
    {
      public:
        void OnCreated() override;

        void SendConnectionRequest();
        void OnServerRemotePacket(const std::vector<uint8_t>& data);
        void OnConnectionAccepted(const std::vector<uint8_t>& data);
        void OnDisconnectAcknowledged(const std::vector<uint8_t>& data);
        void OnServerDisconnected(const std::vector<uint8_t>& data);
    };
}
