#include "interspace/client/Client.hpp"

#include "igneous/networking/NetworkManager.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "igneous/networking/Serializer.hpp"

namespace Interspace::Client
{
    Client::Client(Engine::NetworkInterface* _netInterface, const std::string& _username)
    {
        RegisterMessageHandlers();

        netInterface = _netInterface;
        Engine::NetworkManager::BindMessageHandler(netInterface, this, &Client::OnMessageReceived);

        username = _username;
        RequestConnection(username);
    }

    void Client::OnMessageReceived(const Engine::NetworkMessage& message)
    {
        switch (message.type)
        {
        case Engine::NetworkEventType::Message:
        {
            HandleMessage(message.data);
            break;
        }
        case Engine::NetworkEventType::ClientConnected:
        {
            EmitEvent(CONNECTION_REQUEST, message.data);
            break;
        }
        case Engine::NetworkEventType::ClientDisconnected:
        {
            break;
        }
        case Engine::NetworkEventType::ServerDisconnected:
        {
            EmitEvent(DISCONNECTION_ACKNOWLEDGED, message.data);
            break;
        }
        }
    }

    void Client::RegisterMessageHandlers()
    {
        RegisterMessageHandler(CONNECTION_ACCEPTED, &Client::OnConnectionAccepted);
        RegisterMessageHandler(CLIENT_DISCONNECTED, &Client::OnClientDisconnected);
        RegisterMessageHandler(CLIENT_CONNECTED, &Client::OnClientConnected);
    }

    void Client::RegisterMessageHandler(uint8_t messageType, void (Client::*callback)(const std::vector<uint8_t>&))
    {
        if (messageHandler.contains(messageType))
            return;
        messageHandler.emplace(messageType, std::make_unique<ClientNetEvent>());
        messageHandler[messageType].get()->Connect([this, callback](const std::vector<uint8_t>& data)
                                                   { (this->*callback)(data); });
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

    void Client::HandleMessage(const std::vector<uint8_t>& data)
    {
        if (!netInterface)
            return;
        if (data.empty())
            return;

        uint8_t type = data[0];
        if (messageHandler.contains(type))
            messageHandler[type].get()->Emit(data);

        EmitEvent(type, data);
    }

    std::string Client::GetUsername(client_id_t clientId)
    {
        if (!peers.contains(clientId))
            return "";
        return peers[clientId];
    }
}
