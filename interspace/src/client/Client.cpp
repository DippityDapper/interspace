#include "interspace/client/Client.hpp"

#include "igneous/networking/LocalIdentity.hpp"
#include "interspace/shared/game/Game.hpp"
#include "interspace/shared/network/NetworkPackets.hpp"
#include "igneous/networking/Serializer.hpp"

namespace Interspace::Client
{
    Client::Client(std::unique_ptr<Engine::NetworkInterface> _netInterface, std::unique_ptr<Engine::IdentityProvider> _identity)
    {
        netInterface = std::move(_netInterface);
        identity = std::move(_identity);

        netInterface->onMessageReceived = [this](const Engine::NetworkMessage& message)
        { OnMessageReceived(message); };
    }

    void Client::OnMessageReceived(const Engine::NetworkMessage& message)
    {
        switch (message.type)
        {
        case Engine::NetworkEventType::Message:
        case Engine::NetworkEventType::ClientConnected:
        case Engine::NetworkEventType::ClientDisconnected:
        case Engine::NetworkEventType::ServerDisconnected:
        {
            HandleMessage(message.data);
            break;
        }
        default:;
        }
    }

    void Client::ConnectToEvent(NetMessageType messageType, void (Client::*callback)(const std::vector<uint8_t>&))
    {
        if (!messageHandler.contains(messageType))
            messageHandler.emplace(messageType, std::make_unique<ClientNetEvent>());
        messageHandler[messageType].get()->Connect([this, callback](const std::vector<uint8_t>& data)
                                                   { (this->*callback)(data); });
    }

    void Client::HandleMessage(const std::vector<uint8_t>& data)
    {
        if (!netInterface)
            return;
        if (data.empty())
            return;

        Engine::Deserializer deserializer{data, 0};
        NetMessageType type = static_cast<NetMessageType>(deserializer.ReadUShort());
        if (messageHandler.contains(type))
            messageHandler[type].get()->Emit(data);
    }
}
