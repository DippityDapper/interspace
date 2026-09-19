#include "interspace/server/world/ServerUniverse.hpp"

#include "igneous/networking/Serializer.hpp"
#include "interspace/shared/network/NetworkManager.hpp"

namespace Interspace::Server
{
    void ServerUniverse::OnCreated()
    {
        NetworkManager::server->ConnectToEvent(SERVER_ID_REQUEST, this, &ServerUniverse::OnServerRemoteIdRequest);
        NetworkManager::server->ConnectToEvent(CONNECTION_REQUEST, this, &ServerUniverse::OnClientConnectionRequest);
        NetworkManager::server->ConnectToEvent(DISCONNECTION_REQUEST, this, &ServerUniverse::OnClientDisconnectionRequest);

        NetworkManager::server->identity->OnAuthResult = [this](uint64_t clientId, bool valid)
        { OnAuthResult(clientId, valid); };
    }

    void ServerUniverse::OnServerRemoteIdRequest(const std::vector<uint8_t>& data, uint32_t peerId)
    {
        Engine::Serializer serializer{};
        uint64_t serverId = NetworkManager::server->identity->GetLocalId();
        serializer.Write(static_cast<uint16_t>(SERVER_ID_PACKET));
        serializer.Write(serverId);
        NetworkManager::server->SendToClient(peerId, serializer.GetBytes(), Engine::TransportType::Reliable);
    }

    void ServerUniverse::OnClientConnectionRequest(const std::vector<uint8_t>& data, uint32_t peerId)
    {
        Engine::Deserializer deserializer(data);
        uint64_t clientId = deserializer.ReadULong();
        std::string username = deserializer.ReadString();
        std::vector<uint8_t> token = deserializer.ReadBytes();

        if (pendingAuth.contains(clientId) || NetworkManager::server->GetClientId(peerId) != 0)
        {
            RejectClient(peerId, clientId);
            return;
        }

        AuthPeer authPeer{peerId, username};
        pendingAuth.emplace(clientId, authPeer);
        bool sessionStarted = NetworkManager::server->identity->ValidateToken(clientId, token);
        if (!sessionStarted)
            RejectClient(peerId, clientId);
    }

    void ServerUniverse::OnClientDisconnectionRequest(const std::vector<uint8_t>& data, uint32_t peerId)
    {
        uint64_t clientId = NetworkManager::server->GetClientId(peerId);
        std::string username = NetworkManager::server->GetUsername(peerId);
        NetworkManager::server->identity->OnDisconnect(clientId);
        NetworkManager::server->DisconnectClient(clientId);

        Engine::Serializer serializer{};
        serializer.Write(static_cast<uint16_t>(DISCONNECTION_ACKNOWLEDGED));
        NetworkManager::server->SendToClient(peerId, serializer.GetBytes(), Engine::TransportType::Reliable);

        Engine::Serializer broadcastSerializer{};
        broadcastSerializer.Write(static_cast<uint16_t>(CLIENT_DISCONNECTED));
        broadcastSerializer.Write(clientId);
        broadcastSerializer.Write(username);
        for (uint32_t otherPeerId: NetworkManager::server->GetPeers())
        {
            if (otherPeerId == peerId)
                continue;
            NetworkManager::server->SendToClient(otherPeerId, broadcastSerializer.GetBytes(), Engine::TransportType::Reliable);
        }
    }

    void ServerUniverse::OnAuthResult(uint64_t clientId, bool valid)
    {
        if (!pendingAuth.contains(clientId))
            return;

        AuthPeer authPeer = pendingAuth[clientId];
        pendingAuth.erase(clientId);

        if (!valid)
        {
            RejectClient(authPeer.peerId, clientId);
            return;
        }

        NetworkManager::server->ConnectClient(clientId, authPeer.username, authPeer.peerId);

        Engine::Serializer serializer{};
        serializer.Write(static_cast<uint16_t>(CONNECTION_ACCEPTED));
        NetworkManager::server->SendToClient(authPeer.peerId, serializer.GetBytes(), Engine::TransportType::Reliable);

        Engine::Serializer broadcastSerializer{};
        broadcastSerializer.Write(static_cast<uint16_t>(CLIENT_CONNECTED));
        broadcastSerializer.Write(clientId);
        broadcastSerializer.Write(authPeer.username);
        for (uint32_t peerId: NetworkManager::server->GetPeers())
        {
            if (peerId == authPeer.peerId)
                continue;
            NetworkManager::server->SendToClient(authPeer.peerId, serializer.GetBytes(), Engine::TransportType::Reliable);
        }
    }

    void ServerUniverse::RejectClient(uint32_t peerId, uint64_t clientId)
    {
        if (pendingAuth.contains(clientId))
            pendingAuth.erase(clientId);

        Engine::Serializer serializer{};
        serializer.Write(static_cast<uint16_t>(CONNECTION_REJECTED));
        NetworkManager::server->SendToClient(peerId, serializer.GetBytes(), Engine::TransportType::Reliable);
    }
}
