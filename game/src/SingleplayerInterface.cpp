#include "game/SingleplayerInterface.hpp"

namespace Game
{
    SingleplayerInterface::~SingleplayerInterface()
    {

    }

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

    void SingleplayerInterface::SendToClient(ENetPeer* peer, std::vector<uint8_t> data)
    {
        serverToClient.push(data);
    }

    void SingleplayerInterface::SendToServer(std::vector<uint8_t> data)
    {
        clientToServer.push(data);
    }
}
