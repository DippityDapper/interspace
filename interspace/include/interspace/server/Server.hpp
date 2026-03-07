#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "enet/enet.h"

#include "interspace/server/ServerNetEvent.hpp"
#include "igneous/networking/NetworkInterface.hpp"
#include "interspace/game/Typedefs.hpp"

namespace Interspace::Server
{
    struct ConnectionAttempt
    {
        uint64_t retryTime = 0;
        uint8_t reconnectionAttempt = 0;
    };

    class Server
    {
      private:
        std::unordered_map<uint8_t, std::unique_ptr<ServerNetEvent>> messageHandler{};
        std::unordered_map<uint8_t, std::unique_ptr<ServerNetEvent>> netEvents;

        std::unordered_map<client_id_t, ENetPeer*> peers{};
        std::unordered_map<client_id_t, std::string> idToUsernameLookup{};
        std::unordered_map<ENetPeer*, client_id_t> peerToIdLookup{};

        uint8_t maxConnectionRetryAttempts = 3;
        uint64_t timeoutOffsetInSeconds = 1;
        std::unordered_map<ENetPeer*, ConnectionAttempt> connectionAttempts{};

      public:
        Engine::NetworkInterface* netInterface = nullptr;

      private:
        void OnMessageReceived(const Engine::NetworkMessage& message);
        void AcceptConnectionRequest(_ENetPeer* to, client_id_t clientId);
        void BroadcastConnectionToPeers(client_id_t clientId, const std::string& username);
        void SendPeersToPeer(ENetPeer* to);
        void AcknowledgeDisconnection(ENetPeer* to);
        void BroadcastDisconnectionToPeers(client_id_t clientId);
        void SendConnectionRetryRequest(ENetPeer* to);

      public:
        explicit Server(Engine::NetworkInterface* _netInterface);
        void HandleMessage(const std::vector<uint8_t>& data, ENetPeer* from);
        void CheckConnectionAttempts();

        void RegisterMessageHandlers();
        void RegisterMessageHandler(uint8_t messageType, void (Server::*callback)(const std::vector<uint8_t>& data, ENetPeer* from));

        void CreateNetEvent(uint8_t messageType);
        bool EmitEvent(uint8_t messageType, const std::vector<uint8_t>& data, _ENetPeer* from);

        void OnConnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnDisconnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnUnRequestedConnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnUnRequestedDisconnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from);

        client_id_t ConnectClient(ENetPeer* peer, const std::string& username);
        void DisconnectClient(client_id_t clientId);

        client_id_t GetClientId(const std::string& username);
        client_id_t GetClientId(ENetPeer* peer);
        std::string GetUsername(client_id_t clientId);
        ENetPeer* GetPeer(client_id_t clientId);
        ENetPeer* GetPeer(const std::string& username);
        bool PeerExists(client_id_t clientId);
        std::unordered_map<client_id_t, ENetPeer*> GetPeers();
        bool CheckPeer(client_id_t supposedClientId, ENetPeer* peer);

      public:
        template<typename T>
        ServerNetEvent::Connection ConnectToEvent(uint8_t messageType, T* instance, void (T::*callback)(const std::vector<uint8_t>& data, _ENetPeer* from))
        {
            if (!netEvents.contains(messageType))
                CreateNetEvent(messageType);
            auto netEvent = netEvents[messageType].get();
            return netEvent->Connect(instance, callback);
        }

        template<typename T>
        ServerNetEvent::Connection ConnectToEvent(uint8_t messageType, std::weak_ptr<T> instance, void (T::*callback)(const std::vector<uint8_t>& data, _ENetPeer* from))
        {
            if (!netEvents.contains(messageType))
                CreateNetEvent(messageType);
            auto netEvent = netEvents[messageType].get();
            return netEvent->Connect(instance, callback);
        }

        template<typename Callable>
        ServerNetEvent::Connection ConnectToEvent(uint8_t messageType, Callable&& func)
        {
            if (!netEvents.contains(messageType))
                CreateNetEvent(messageType);
            auto netEvent = netEvents[messageType].get();
            return netEvent->Connect(func);
        }

        bool DisconnectFromEvent(uint8_t messageType, const ServerNetEvent::Connection& connection)
        {
            if (!netEvents.contains(messageType))
                return false;
            auto netEvent = netEvents[messageType].get();
            netEvent->Disconnect(connection);
            return true;
        }
    };
}
