#pragma once

#include "igneous/networking/LocalIdentity.hpp"

#include <unordered_map>
#include <vector>
#include <memory>

#include "interspace/client/network/ClientNetEvent.hpp"
#include "igneous/networking/NetworkInterface.hpp"
#include "interspace/shared/network/NetworkPackets.hpp"

namespace Interspace::Client
{
    class Client
    {
      private:
        std::unordered_map<NetMessageType, std::unique_ptr<ClientNetEvent>> messageHandler{};

      public:
        std::unique_ptr<Engine::NetworkInterface> netInterface = nullptr;
        std::unique_ptr<Engine::IdentityProvider> identity = nullptr;

      private:
        void OnMessageReceived(const Engine::NetworkMessage& message);

      public:
        Client(std::unique_ptr<Engine::NetworkInterface> _netInterface, std::unique_ptr<Engine::IdentityProvider> _identity);

        uint64_t GetMyId() const;
        std::string GetMyUsername() const;

        void SendToServer(const std::vector<uint8_t>& data, uint32_t flags);

        template<typename T>
        void ConnectToEvent(NetMessageType messageType, T* instance, void (T::*callback)(const std::vector<uint8_t>& data))
        {
            if (!messageHandler.contains(messageType))
                messageHandler.emplace(messageType, std::make_unique<ClientNetEvent>());
            messageHandler[messageType]->Connect(instance, callback);
        }
    };
}
