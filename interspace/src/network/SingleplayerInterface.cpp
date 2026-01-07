#include "interspace/network/SingleplayerInterface.hpp"

namespace Interspace
{
    void SingleplayerInterface::Poll()
    {
        while (!clientToServer.empty() && serverMessageHandler != nullptr)
        {
            std::vector<uint8_t> msg = clientToServer.front();
            clientToServer.pop();
            serverMessageHandler(msg, nullptr);
        }

        while (!serverToClient.empty() && clientMessageHandler != nullptr)
        {
            std::vector<uint8_t> msg = serverToClient.front();
            serverToClient.pop();
            clientMessageHandler(msg);
        }
    }

    bool SingleplayerInterface::Connected()
    {
        return true;
    }

    void SingleplayerInterface::SendToClient(ENetPeer* peer, std::vector<uint8_t> data, enet_uint32 packetType)
    {
        serverToClient.push(data);
    }

    void SingleplayerInterface::SendToServer(std::vector<uint8_t> data, enet_uint32 packetType)
    {
        clientToServer.push(data);
    }
}
