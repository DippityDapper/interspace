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

        std::unordered_map<uint64_t, uint32_t> clientIdToPeerId{};
        std::unordered_map<uint32_t, uint64_t> peerIdToClientId{};
        std::unordered_map<uint32_t, std::string> peerIdToUsername{};

      public:
        std::unique_ptr<Engine::NetworkInterface> netInterface = nullptr;
        std::unique_ptr<Engine::IdentityProvider> identity = nullptr;

      private:
        void OnMessageReceived(const Engine::NetworkMessage& message);

      public:
        explicit Server(std::unique_ptr<Engine::NetworkInterface> _netInterface, std::unique_ptr<Engine::IdentityProvider> _identity);

        void SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, uint32_t flags);

        void ConnectClient(uint64_t clientId, const std::string& username, uint32_t peerId);
        void DisconnectClient(uint64_t clientId);
        uint64_t GetClientId(uint32_t peerId);
        std::string GetUsername(uint32_t peerId);
        std::vector<uint32_t> GetPeers();

        template<typename T>
        void ConnectToEvent(NetMessageType messageType, T* instance, void (T::*callback)(const std::vector<uint8_t>& data, uint32_t peerId))
        {
            if (!messageHandler.contains(messageType))
                messageHandler.emplace(messageType, std::make_unique<ServerNetEvent>());
            messageHandler[messageType]->Connect(instance, callback);
        }
    };
}
