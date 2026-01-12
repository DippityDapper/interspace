#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "interspace/client/ClientNetEvent.hpp"
#include "interspace/network/NetInterface.hpp"

namespace Interspace::Client
{
    class Client
    {
    private:
        std::unordered_map<uint8_t, std::unique_ptr<ClientNetEvent>> messageHandler{};
        std::unordered_map<uint8_t, std::unique_ptr<ClientNetEvent>> netEvents{};

        std::unordered_map<uint32_t, std::string> peers{};

    public:
        NetInterface* netInterface = nullptr;

        std::string username;
        uint32_t clientId = 0;

    public:
        Client(NetInterface* _netInterface, const std::string& _username);
        void HandleMessage(std::vector<uint8_t>);

        void RegisterMessageHandlers();
        void RegisterMessageHandler(uint8_t messageType, void(Client::* callback)(const std::vector<uint8_t>& data));

        void CreateNetEvent(uint8_t messageType);
        bool EmitEvent(uint8_t messageType, const std::vector<uint8_t>& data);

        void HandleConnectionAccepted(const std::vector<uint8_t>& data);
        void HandleClientDisconnected(const std::vector<uint8_t>& data);
        void HandlePeerConnected(const std::vector<uint8_t>& data);

        std::string GetUsername(uint32_t peerId);

    public:
        template <typename T>
        ClientNetEvent::Connection ConnectToEvent(uint8_t messageType, T* instance, void(T::* callback)(const std::vector<uint8_t>& data))
        {
            if (!netEvents.contains(messageType))
                CreateNetEvent(messageType);
            auto netEvent = netEvents[messageType].get();
            return netEvent->Connect(instance, callback);
        }

        template <typename T>
        ClientNetEvent::Connection ConnectToEvent(uint8_t messageType, std::weak_ptr<T> instance, void(T::* callback)(const std::vector<uint8_t>& data))
        {
            if (!netEvents.contains(messageType))
                CreateNetEvent(messageType);
            auto netEvent = netEvents[messageType].get();
            return netEvent->Connect(instance, callback);
        }

        template <typename Callable>
        ClientNetEvent::Connection ConnectToEvent(uint8_t messageType, Callable&& func)
        {
            if (!netEvents.contains(messageType))
                CreateNetEvent(messageType);
            auto netEvent = netEvents[messageType].get();
            return netEvent->Connect(func);
        }

        bool DisconnectFromEvent(uint8_t messageType, const ClientNetEvent::Connection& connection)
        {
            if (!netEvents.contains(messageType))
                return false;
            auto netEvent = netEvents[messageType].get();
            netEvent->Disconnect(connection);
            return true;
        }
    };
}
