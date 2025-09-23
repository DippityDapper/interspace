#pragma once

#include <map>
#include <functional>
#include <random>
#include <queue>
#include <vector>

#include "SDL3/SDL.h"
#include "enet/enet.h"

#include "common/packets/Packets.h"

namespace Engine
{
    class NetworkManager
    {
    public:
        std::map<int, ENetPeer*> peers{};
        ENetHost* server = nullptr;

        static std::map<PacketType, std::vector<std::function<void(ENetEvent&)>>> packetHandlers;
        static std::vector<std::function<void(int, NetworkManager*)>> connectionEvent;
        static std::vector<std::function<void(int, NetworkManager*)>> disconnectionEvent;

        std::queue<int> newlyConnectedClients{};

    public:
        SDL_AppResult Init();
        void InitPacketHandlers();
        static void AddHandler(PacketType pType, const std::function<void(ENetEvent&)>& func);

        SDL_AppResult CreateServer(int port, bool localHostOnly);
        void DisconnectClient(int clientId);

        void HandleNetworkEvents();
        void HandlePackets(ENetEvent enetEvent);

        void HandleConnectionPacket(ENetEvent& enetEvent);
        void HandleDisconnectPacket(ENetEvent& enetEvent);
    };
}
