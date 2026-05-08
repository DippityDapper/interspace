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
        case Engine::NetworkEventType::ServerDisconnected:
        {
            NetMessageType type = SERVER_DISCONNECTED;
            if (messageHandler.contains(type))
                messageHandler[type]->Emit(message.data);
            break;
        }
        case Engine::NetworkEventType::Message:
        {
            Engine::Deserializer deserializer{message.data, 0};
            NetMessageType type = static_cast<NetMessageType>(deserializer.ReadUShort());
            if (messageHandler.contains(type))
                messageHandler[type].get()->Emit(message.data);
            break;
        }
        default:;
        }
    }

    uint64_t Client::GetMyId() const
    {
        if (!identity)
            return 0;
        if (identity->GetLocalId() <= 0)
            identity->GenerateLocalId();
        return identity->GetLocalId();
    }

    std::string Client::GetMyUsername() const
    {
        if (!identity)
            return "";
        if (identity->GetLocalUsername().empty())
            identity->GenerateLocalUsername();
        return identity->GetLocalUsername();
    }

    void Client::SendToServer(const std::vector<uint8_t>& data, uint32_t flags)
    {
        if (!netInterface)
            return;

        netInterface->SendToServer(data, flags);
    }
}
