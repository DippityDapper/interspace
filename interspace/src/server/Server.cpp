#include "interspace/server/Server.hpp"

#include "SDL3/SDL_log.h"

#include <string>

#include "igneous/networking/NetworkManager.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/network/NetworkPackets.hpp"

#include <random>

namespace Interspace::Server
{
    Server::Server(Engine::NetworkInterface* _netInterface)
    {
        RegisterMessageHandlers();

        netInterface = _netInterface;
        Engine::NetworkManager::BindMessageHandler(netInterface, this, &Server::OnMessageReceived);
    }

    void Server::OnMessageReceived(const Engine::NetworkMessage& message)
    {
        switch (message.type)
        {
        case Engine::NetworkEventType::Message:
        {
            HandleMessage(message.data, message.peer);
            break;
        }
        case Engine::NetworkEventType::ClientConnected:
        {
            EmitEvent(CONNECTION_REQUEST_, message.data, message.peer);
            break;
        }
        case Engine::NetworkEventType::ClientDisconnected:
        {
            EmitEvent(DISCONNECTION_REQUEST_, message.data, message.peer);
            break;
        }
        case Engine::NetworkEventType::ServerDisconnected:
        {
            break;
        }
        }
    }

    void Server::RegisterMessageHandlers()
    {
        RegisterMessageHandler(CONNECTION_REQUEST, &Server::OnConnectionRequest);
        RegisterMessageHandler(DISCONNECTION_REQUEST, &Server::OnDisconnectionRequest);
        RegisterMessageHandler(CONNECTION_REQUEST_, &Server::OnUnRequestedConnectionRequest);
        RegisterMessageHandler(DISCONNECTION_REQUEST_, &Server::OnUnRequestedDisconnectionRequest);
    }

    void Server::RegisterMessageHandler(uint8_t messageType, void (Server::*callback)(const std::vector<uint8_t>& data, ENetPeer* from))
    {
        if (messageHandler.contains(messageType))
            return;
        messageHandler.emplace(messageType, std::make_unique<ServerNetEvent>());
        messageHandler[messageType].get()->Connect([this, callback](const std::vector<uint8_t>& data, ENetPeer* from)
                                                   { (this->*callback)(data, from); });
    }

    void Server::CreateNetEvent(uint8_t messageType)
    {
        if (!netEvents.contains(messageType))
            netEvents.emplace(messageType, std::make_unique<ServerNetEvent>());
    }

    bool Server::EmitEvent(uint8_t messageType, const std::vector<uint8_t>& data, _ENetPeer* from)
    {
        if (!netEvents.contains(messageType))
            return false;
        netEvents[messageType]->Emit(data, from);
        return true;
    }

    void Server::HandleMessage(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        if (!netInterface)
            return;
        if (data.empty())
            return;

        uint8_t type = data[0];
        if (messageHandler.contains(type))
            messageHandler[type].get()->Emit(data, from);

        EmitEvent(type, data, from);
    }

    void Server::CheckConnectionAttempts()
    {
        if (connectionAttempts.empty())
            return;

        auto it = connectionAttempts.begin();
        for (; it != connectionAttempts.end(); ++it)
        {
            ConnectionAttempt connectionAttempt = it->second;

            uint64_t currentTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            if (currentTime < connectionAttempt.retryTime)
                continue;

            connectionAttempt.reconnectionAttempt++;
            if (connectionAttempt.reconnectionAttempt >= maxConnectionRetryAttempts)
            {
                AcknowledgeDisconnection(it->first);
                connectionAttempts.erase(it->first);
                continue;
            }
            connectionAttempt.retryTime = currentTime + timeoutOffsetInSeconds;
            SendConnectionRetryRequest(it->first);
        }
    }

    void Server::SendConnectionRetryRequest(ENetPeer* to)
    {
    }

    client_id_t Server::ConnectClient(ENetPeer* peer, const std::string& username)
    {
        if (!DBHelper::PlayerExists(username))
        {
            std::mt19937 gen(std::random_device{}());

            client_id_t clientId = 0;
            do
            {
                std::uniform_int_distribution<client_id_t> peerIdDist(1, std::numeric_limits<client_id_t>::max());
                clientId = peerIdDist(gen);
            } while (DBHelper::PlayerExists(clientId));

            DBHelper::InsertPlayer(clientId, username);
        }

        client_id_t newId = DBHelper::GetclientId(username);

        if (!peers.contains(newId))
            peers.emplace(newId, peer);
        if (!idToUsernameLookup.contains(newId))
            idToUsernameLookup.emplace(newId, username);
        if (!peerToIdLookup.contains(peer))
            peerToIdLookup.emplace(peer, newId);

        if (connectionAttempts.contains(peer))
            connectionAttempts.erase(peer);

        return newId;
    }

    void Server::DisconnectClient(client_id_t clientId)
    {
        ENetPeer* peer = GetPeer(clientId);

        if (peers.contains(clientId))
            peers.erase(clientId);
        if (idToUsernameLookup.contains(clientId))
            idToUsernameLookup.erase(clientId);
        if (peer && peerToIdLookup.contains(peer))
            peerToIdLookup.erase(peer);
        if (connectionAttempts.contains(peer))
            connectionAttempts.erase(peer);
    }

    client_id_t Server::GetClientId(const std::string& username)
    {
        for (const auto& client: idToUsernameLookup)
        {
            if (client.second == username)
                return client.first;
        }
        return 0;
    }

    client_id_t Server::GetClientId(ENetPeer* peer)
    {
        if (peerToIdLookup.contains(peer))
            return peerToIdLookup[peer];
        return 0;
    }

    std::string Server::GetUsername(client_id_t clientId)
    {
        if (!idToUsernameLookup.contains(clientId))
            return "";
        return idToUsernameLookup[clientId];
    }

    ENetPeer* Server::GetPeer(client_id_t clientId)
    {
        if (!peers.contains(clientId))
            return nullptr;
        return peers[clientId];
    }

    ENetPeer* Server::GetPeer(const std::string& username)
    {
        client_id_t clientId = GetClientId(username);
        return GetPeer(clientId);
    }

    bool Server::PeerExists(client_id_t clientId)
    {
        return peers.contains(clientId);
    }

    std::unordered_map<client_id_t, ENetPeer*> Server::GetPeers()
    {
        std::unordered_map<client_id_t, ENetPeer*> result{};
        for (auto& peer: peers)
            result.emplace(peer.first, peer.second);
        return result;
    }

    bool Server::CheckPeer(client_id_t supposedClientId, ENetPeer* peer)
    {
        client_id_t realClientId = GetClientId(peer);
        if (supposedClientId == realClientId)
            return true;
        return false;
    }
}
