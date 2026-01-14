#include "interspace/server/Server.hpp"

#include <string>

#include "igneous/networking/NetworkManager.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "SDL3/SDL_log.h"

namespace Interspace::Server
{
    Server::Server(Engine::NetworkInterface* _netInterface)
    {
        RegisterMessageHandlers();

        netInterface = _netInterface;
        Engine::NetworkManager::BindMessageHandler(netInterface, this, &Server::OnMessageReceived);
    }

    void Server::OnMessageReceived(const Engine::NetworkMessage &message)
    {
        switch (message.type)
        {
            case Engine::NetworkEventType::Message:
            {
                HandleMessage(message.data, message.peer);
                break;
            }
            case Engine::NetworkEventType::ClientConnected:
            {

                EmitEvent(CONNECTION_REQUEST_, message.data, message.peer);
                break;
            }
            case Engine::NetworkEventType::ClientDisconnected:
            {
                EmitEvent(DISCONNECTION_REQUEST_, message.data, message.peer);
                break;
            }
            case Engine::NetworkEventType::ServerDisconnected:
            {
                break;
            }
        }
    }

    void Server::RegisterMessageHandlers()
    {
        RegisterMessageHandler(CONNECTION_REQUEST, &Server::HandleConnectionRequest);
        RegisterMessageHandler(DISCONNECTION_REQUEST, &Server::HandleDisconnectionRequest);
        RegisterMessageHandler(CONNECTION_REQUEST_, &Server::HandleUnRequestedConnectionRequest);
        RegisterMessageHandler(DISCONNECTION_REQUEST_, &Server::HandleUnRequestedDisconnectionRequest);
    }

    void Server::RegisterMessageHandler(uint8_t messageType, void(Server::* callback)(const std::vector<uint8_t>& data, ENetPeer* from))
    {
        if (messageHandler.contains(messageType))
            return;
        messageHandler.emplace(messageType, std::make_unique<ServerNetEvent>());
        messageHandler[messageType].get()->Connect([this, callback](const std::vector<uint8_t>& data, ENetPeer* from){(this->*callback)(data, from);});
    }

    void Server::CreateNetEvent(uint8_t messageType)
    {
        if (!netEvents.contains(messageType))
            netEvents.emplace(messageType, std::make_unique<ServerNetEvent>());
    }

    bool Server::EmitEvent(uint8_t messageType, const std::vector<uint8_t>& data, _ENetPeer* from)
    {
        if (!netEvents.contains(messageType))
            return false;
        netEvents[messageType]->Emit(data, from);
        return true;
    }

    void Server::HandleMessage(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        if (!netInterface)
            return;
        if (data.empty())
            return;

        uint8_t type = data[0];
        if (messageHandler.contains(type))
            messageHandler[type].get()->Emit(data, from);

        EmitEvent(type, data, from);
    }

    uint32_t Server::GetUserId(const std::string& username)
    {
        for (const auto& user : idToUsernameLookup)
        {
            if (user.second == username)
                return user.first;
        }
        return 0;
    }

    std::string Server::GetUsername(uint32_t peerId)
    {
        if (!idToUsernameLookup.contains(peerId))
            return "";
        return idToUsernameLookup[peerId];
    }

    ENetPeer* Server::GetPeer(uint32_t peerId)
    {
        if (!peers.contains(peerId))
            return nullptr;
        return peers[peerId];
    }

    ENetPeer* Server::GetPeer(const std::string& username)
    {
        uint32_t peerId = GetUserId(username);
        return GetPeer(peerId);
    }

    bool Server::PeerExists(uint32_t peerId)
    {
        return peers.contains(peerId);
    }

    std::unordered_map<uint32_t, ENetPeer*> Server::GetPeers()
    {
        std::unordered_map<uint32_t, ENetPeer*> result{};
        for (auto& peer : peers)
            result.emplace(peer.first, peer.second);
        return result;
    }
}
