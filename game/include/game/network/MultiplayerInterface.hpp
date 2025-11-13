#pragma once

#include <string>
#include <thread>
#include <unordered_map>

#include "enet/enet.h"

#include "dapper2d/ThreadSafeQueue.hpp"

#include "game/network/NetInterface.hpp"

namespace Game
{
    struct NetworkMessage
    {
        ENetPeer* peer = nullptr;
        std::vector<uint8_t> data{};
    };

    class MultiplayerInterface : public NetInterface
    {
    private:
        bool isServer = false;
        bool running = false;

        ENetHost* host = nullptr;
        ENetPeer* serverPeer = nullptr;

        std::thread networkThread;
        ThreadSafeQueue<NetworkMessage> toNetwork;
        ThreadSafeQueue<NetworkMessage> fromNetwork;

    private:
        void NetworkLoop();

    public:
        MultiplayerInterface(int port, int peerCount, bool localOnly);
        MultiplayerInterface(int port, const std::string& ip);
        ~MultiplayerInterface() override;

        void SendToServer(std::vector<uint8_t> data) override;
        void SendToClient(ENetPeer* peer, std::vector<uint8_t> data) override;
        void Poll() override;
        bool Connected() override;

        void HandleServerEvent(const ENetEvent& event);
        void HandleClientEvent(const ENetEvent& event);

        void SendPacket(ENetPeer* peer, std::vector<uint8_t> data);
    };
}
