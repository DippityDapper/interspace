#include "interspace/server/Server.hpp"

#include "igneous/networking/Serializer.hpp"

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
        case Engine::NetworkEventType::Message:
        case Engine::NetworkEventType::ClientConnected:
        case Engine::NetworkEventType::ClientDisconnected:
        {
            HandleMessage(message.data, message.peerId);
            break;
        }
        default:;
        }
    }

    void Server::ConnectToEvent(NetMessageType messageType, void (Server::*callback)(const std::vector<uint8_t>& data, uint32_t peerId))
    {
        if (!messageHandler.contains(messageType))
            messageHandler.emplace(messageType, std::make_unique<ServerNetEvent>());
        messageHandler[messageType]->Connect([this, callback](const std::vector<uint8_t>& data, uint32_t peerId)
                                             { (this->*callback)(data, peerId); });
    }

    void Server::HandleMessage(const std::vector<uint8_t>& data, uint32_t peerId)
    {
        if (!netInterface)
            return;
        if (data.empty())
            return;

        Engine::Deserializer deserializer{data, 0};
        NetMessageType type = static_cast<NetMessageType>(deserializer.ReadUShort());
        if (messageHandler.contains(type))
            messageHandler[type].get()->Emit(data, peerId);
    }
}
