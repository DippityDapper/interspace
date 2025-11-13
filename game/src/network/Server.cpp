#include "game/network/Server.hpp"

#include <cstring>
#include <string>

#include "SDL3/SDL_log.h"

#include "game/network/NetworkPackets.hpp"

namespace Game
{
    Server::Server(NetInterface* _netInterface)
    {
        RegisterMessageHandlers();

        netInterface = _netInterface;
        netInterface->serverMessageHandler = [this](const std::vector<uint8_t>& data, ENetPeer* peer)
        {
            this->HandleMessage(data, peer);
        };
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
        messageHandler.emplace(messageType, [this, callback](const std::vector<uint8_t>& data, ENetPeer* from){(this->*callback)(data, from);});
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
            messageHandler[type](data, from);

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
}
