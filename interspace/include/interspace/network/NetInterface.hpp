#pragma once

#include <functional>

#include "enet/enet.h"

namespace Interspace
{
    class NetInterface
    {
    public:
        std::function<void(std::vector<uint8_t>, ENetPeer*)> serverMessageHandler;
        std::function<void(std::vector<uint8_t>)> clientMessageHandler;

    public:
        virtual ~NetInterface() = default;
        virtual void SendToServer(std::vector<uint8_t> data, enet_uint32 packetType) = 0;
        virtual void SendToClient(ENetPeer* peer, std::vector<uint8_t> data, enet_uint32 packetType) = 0;
        virtual void Poll() = 0;
        virtual bool Connected() = 0;
    };
}