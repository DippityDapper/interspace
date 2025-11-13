#include "game/client/Client.hpp"

#include "SDL3/SDL_log.h"

#include "game/game/Game.hpp"
#include "game/network/NetworkPackets.hpp"

namespace Game
{
    Client::Client(NetInterface* _netInterface, const std::string& _username)
    {
        RegisterMessageHandlers();

        netInterface = _netInterface;
        netInterface->clientMessageHandler = [this](const std::vector<uint8_t>& data)
        {
            this->HandleMessage(data);
        };

        username = _username;

        std::vector<uint8_t> msg;
        msg.push_back(CONNECTION_REQUEST);

        uint32_t usernameLen = static_cast<uint32_t>(username.size());
        PackBytes(msg, &usernameLen, sizeof(uint32_t));
        PackBytes(msg, username.data(), usernameLen);

        netInterface->SendToServer(msg);
    }

    void Client::RegisterMessageHandlers()
    {
        RegisterMessageHandler(CONNECTION_ACCEPTED, &Client::HandleConnectionAccepted);
        RegisterMessageHandler(CLIENT_DISCONNECTED, &Client::HandleClientDisconnected);
        RegisterMessageHandler(CLIENT_CONNECTED, &Client::HandlePeerConnected);
    }

    void Client::RegisterMessageHandler(uint8_t messageType, void(Client::* callback)(const std::vector<uint8_t>&))
    {
        if (messageHandler.contains(messageType))
            return;
        messageHandler.emplace(messageType, [this, callback](const std::vector<uint8_t>& data){(this->*callback)(data);});
    }

    void Client::CreateNetEvent(uint8_t messageType)
    {
        if (!netEvents.contains(messageType))
            netEvents.emplace(messageType, std::make_unique<ClientNetEvent>());
    }

    bool Client::EmitEvent(uint8_t messageType, const std::vector<uint8_t>& data)
    {
        if (!netEvents.contains(messageType))
            return false;
        netEvents[messageType]->Emit(data);
        return true;
    }

    void Client::HandleMessage(std::vector<uint8_t> data)
    {
        if (!netInterface)
            return;
        if (data.empty())
            return;

        uint8_t type = data[0];
        if (messageHandler.contains(type))
            messageHandler[type](data);

        EmitEvent(type, data);
    }

    std::string Client::GetUsername(uint32_t peerId)
    {
        if (!peers.contains(peerId))
            return "";
        return peers[peerId];
    }
}
