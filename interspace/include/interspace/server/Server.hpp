#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "enet/enet.h"

#include "interspace/server/ServerNetEvent.hpp"
#include "interspace/network/NetInterface.hpp"

namespace Interspace::Server
{
    class Server
    {
    private:
        std::unordered_map<uint8_t, std::unique_ptr<ServerNetEvent>> messageHandler{};
        std::unordered_map<uint8_t, std::unique_ptr<ServerNetEvent>> netEvents;

        std::unordered_map<uint32_t, ENetPeer*> peers{};
        std::unordered_map<uint32_t, std::string> idToUsernameLookup{};

    public:
        NetInterface* netInterface = nullptr;

    public:
        explicit Server(NetInterface* _netInterface);
        void HandleMessage(const std::vector<uint8_t>& data, ENetPeer* from);

        void RegisterMessageHandlers();
        void RegisterMessageHandler(uint8_t messageType, void(Server::* callback)(const std::vector<uint8_t>& data, ENetPeer* from));

        void CreateNetEvent(uint8_t messageType);
        bool EmitEvent(uint8_t messageType, const std::vector<uint8_t>& data, _ENetPeer* from);

        void HandleConnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from);
        void HandleDisconnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from);
        void HandleUnRequestedConnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from);
        void HandleUnRequestedDisconnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from);

        uint32_t GetUserId(const std::string& username);
        std::string GetUsername(uint32_t peerId);
        ENetPeer* GetPeer(uint32_t peerId);
        ENetPeer* GetPeer(const std::string& username);
        bool PeerExists(uint32_t peerId);
        std::unordered_map<uint32_t, ENetPeer*> GetPeers();

    public:
        template <typename T>
        ServerNetEvent::Connection ConnectToEvent(uint8_t messageType, T* instance, void(T::* callback)(const std::vector<uint8_t>& data, _ENetPeer* from))
        {
            if (!netEvents.contains(messageType))
                CreateNetEvent(messageType);
            auto netEvent = netEvents[messageType].get();
            return netEvent->Connect(instance, callback);
        }

        template <typename T>
        ServerNetEvent::Connection ConnectToEvent(uint8_t messageType, std::weak_ptr<T> instance, void(T::* callback)(const std::vector<uint8_t>& data, _ENetPeer* from))
        {
            if (!netEvents.contains(messageType))
                CreateNetEvent(messageType);
            auto netEvent = netEvents[messageType].get();
            return netEvent->Connect(instance, callback);
        }

        template <typename Callable>
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
