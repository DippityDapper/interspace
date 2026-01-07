#pragma once

#include <queue>
#include <cstdint>

#include "interspace/network/NetInterface.hpp"

namespace Interspace
{
    class SingleplayerInterface : public NetInterface
    {
    private:
        std::queue<std::vector<uint8_t>> clientToServer;
        std::queue<std::vector<uint8_t>> serverToClient;

    public:
        ~SingleplayerInterface() override = default;
        void SendToServer(std::vector<uint8_t> data, enet_uint32 packetType) override;
        void SendToClient(ENetPeer* peer, std::vector<uint8_t> data, enet_uint32 packetType) override;
        void Poll() override;
        bool Connected() override;
    };
}
