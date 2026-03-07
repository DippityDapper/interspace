#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "interspace/client/ClientNetEvent.hpp"
#include "igneous/networking/NetworkInterface.hpp"
#include "interspace/game/Typedefs.hpp"

namespace Interspace::Client
{
    class Client
    {
      private:
        std::unordered_map<uint8_t, std::unique_ptr<ClientNetEvent>> messageHandler{};
        std::unordered_map<uint8_t, std::unique_ptr<ClientNetEvent>> netEvents{};

        std::unordered_map<client_id_t, std::string> peers{};

      public:
        Engine::NetworkInterface* netInterface = nullptr;

        std::string username;
        client_id_t clientId = 0;

      private:
        void OnMessageReceived(const Engine::NetworkMessage& message);
        void RequestConnection(const std::string& _username);

      public:
        Client(Engine::NetworkInterface* _netInterface, const std::string& _username);
        void HandleMessage(const std::vector<uint8_t>& data);

        void RegisterMessageHandlers();
        void RegisterMessageHandler(uint8_t messageType, void (Client::*callback)(const std::vector<uint8_t>& data));

        void CreateNetEvent(uint8_t messageType);
        bool EmitEvent(uint8_t messageType, const std::vector<uint8_t>& data);

        void OnConnectionAccepted(const std::vector<uint8_t>& data);
        void OnClientDisconnected(const std::vector<uint8_t>& data);
        void OnClientConnected(const std::vector<uint8_t>& data);

        std::string GetUsername(client_id_t clientId);

      public:
        template<typename T>
        ClientNetEvent::Connection ConnectToEvent(uint8_t messageType, T* instance, void (T::*callback)(const std::vector<uint8_t>& data))
        {
            if (!netEvents.contains(messageType))
                CreateNetEvent(messageType);
            auto netEvent = netEvents[messageType].get();
            return netEvent->Connect(instance, callback);
        }

        template<typename T>
        ClientNetEvent::Connection ConnectToEvent(uint8_t messageType, std::weak_ptr<T> instance, void (T::*callback)(const std::vector<uint8_t>& data))
        {
            if (!netEvents.contains(messageType))
                CreateNetEvent(messageType);
            auto netEvent = netEvents[messageType].get();
            return netEvent->Connect(instance, callback);
        }

        template<typename Callable>
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
