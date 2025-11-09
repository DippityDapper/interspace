#pragma once

#include "game/NetInterface.hpp"

namespace Game
{
    class SingleplayerInterface : public NetInterface
    {
    private:
        std::queue<std::vector<uint8_t>> clientToServer;
        std::queue<std::vector<uint8_t>> serverToClient;

    public:
        ~SingleplayerInterface() override;
        void SendToServer(std::vector<uint8_t> data) override;
        void SendToClient(ENetPeer* peer, std::vector<uint8_t> data) override;
        void Poll() override;
    };
}
