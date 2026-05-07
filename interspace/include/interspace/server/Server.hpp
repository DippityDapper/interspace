#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "igneous/networking/LocalIdentity.hpp"

#include "network/ServerNetEvent.hpp"
#include "igneous/networking/NetworkInterface.hpp"
#include "interspace/shared/network/NetworkPackets.hpp"

namespace Interspace::Server
{
    class Server
    {
      private:
        std::unordered_map<NetMessageType, std::unique_ptr<ServerNetEvent>> messageHandler{};

      public:
        std::unique_ptr<Engine::NetworkInterface> netInterface = nullptr;
        std::unique_ptr<Engine::IdentityProvider> identity = nullptr;

      private:
        void OnMessageReceived(const Engine::NetworkMessage& message);

      public:
        explicit Server(std::unique_ptr<Engine::NetworkInterface> _netInterface, std::unique_ptr<Engine::IdentityProvider> _identity);
        void HandleMessage(const std::vector<uint8_t>& data, uint32_t peerId);
        void ConnectToEvent(NetMessageType messageType, void (Server::*callback)(const std::vector<uint8_t>& data, uint32_t peerId));
    };
}
