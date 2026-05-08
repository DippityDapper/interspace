#include "interspace/server/Server.hpp"

#include "SDL3/SDL_log.h"
#include "igneous/networking/Serializer.hpp"

#include <ranges>

namespace Interspace::Server
{
    Server::Server(std::unique_ptr<Engine::NetworkInterface> _netInterface, std::unique_ptr<Engine::IdentityProvider> _identity)
    {
        netInterface = std::move(_netInterface);
        identity = std::move(_identity);

        netInterface->onMessageReceived = [this](const Engine::NetworkMessage& message)
        { OnMessageReceived(message); };
    }

    void Server::OnMessageReceived(const Engine::NetworkMessage& message)
    {
        switch (message.type)
        {
        case Engine::NetworkEventType::ClientConnected:
        {
            NetMessageType type = CONNECTION_REQUEST_;
            if (messageHandler.contains(type))
                messageHandler[type]->Emit(message.data, message.peerId);
            break;
        }
        case Engine::NetworkEventType::ClientDisconnected:
        {
            NetMessageType type = DISCONNECTION_REQUEST_;
            if (messageHandler.contains(type))
                messageHandler[type]->Emit(message.data, message.peerId);
            break;
        }
        case Engine::NetworkEventType::Message:
        {
            Engine::Deserializer deserializer{message.data, 0};
            NetMessageType type = static_cast<NetMessageType>(deserializer.ReadUShort());
            if (messageHandler.contains(type))
                messageHandler[type].get()->Emit(message.data, message.peerId);
            break;
        }
        default:;
        }
    }

    void Server::SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, uint32_t flags)
    {
        if (!netInterface)
            return;
        netInterface->SendToClient(peerId, data, flags);
    }

    void Server::ConnectClient(uint64_t clientId, const std::string& username, uint32_t peerId)
    {
        if (clientIdToPeerId.contains(clientId))
            return;
        clientIdToPeerId.emplace(clientId, peerId);
        peerIdToClientId.emplace(peerId, clientId);
        peerIdToUsername.emplace(peerId, username);
        SDL_Log("[Server] %s (%u) Connected.", username.c_str(), clientId);
    }

    void Server::DisconnectClient(uint64_t clientId)
    {
        if (!clientIdToPeerId.contains(clientId))
            return;
        uint32_t peerId = clientIdToPeerId[clientId];
        std::string username = GetUsername(peerId);
        clientIdToPeerId.erase(clientId);
        peerIdToClientId.erase(peerId);
        peerIdToUsername.erase(peerId);
        SDL_Log("[Server] %s (%u) Disconnected.", username.c_str(), clientId);
    }
    uint64_t Server::GetClientId(uint32_t peerId)
    {
        if (!peerIdToClientId.contains(peerId))
            return 0;
        return peerIdToClientId[peerId];
    }

    std::string Server::GetUsername(uint32_t peerId)
    {
        if (!peerIdToUsername.contains(peerId))
            return "";
        return peerIdToUsername[peerId];
    }

    std::vector<uint32_t> Server::GetPeers()
    {
        std::vector<uint32_t> peers{};
        for (uint32_t peerId: peerIdToClientId | std::views::keys)
            peers.push_back(peerId);
        return peers;
    }
}
