#include "game/server/Server.hpp"

#include "SDL3/SDL_log.h"

#include "game/network/NetworkPackets.hpp"

namespace Game
{
    void Server::HandleConnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        size_t offset = 1; // start after the request

        uint32_t usernameLength = UnpackUint32(data, offset);
        std::string username = UnpackString(data, offset, usernameLength);

        uint32_t newId = nextPeerId++;

        std::vector<uint8_t> response;
        response.push_back(CONNECTION_ACCEPTED);
        PackBytes(response, &newId, sizeof(uint32_t));
        netInterface->SendToClient(from, response);

        std::vector<uint8_t> newClientData;
        newClientData.push_back(CLIENT_CONNECTED);

        uint32_t usernameLen = username.size();
        PackBytes(newClientData, &newId, sizeof(uint32_t));
        PackBytes(newClientData, &usernameLen, sizeof(uint32_t));
        PackBytes(newClientData, username.data(), usernameLen);

        for (const auto& peer : peers)
        {
            netInterface->SendToClient(peer.second, newClientData);

            std::vector<uint8_t> peerData;
            peerData.push_back(CLIENT_CONNECTED);

            uint32_t peerId = peer.first;
            std::string peerUsername = idToUsernameLookup[peerId];
            uint32_t peerUsernameLen = peerUsername.size();
            PackBytes(peerData, &peerId, sizeof(uint32_t));
            PackBytes(peerData, &peerUsernameLen, sizeof(uint32_t));
            PackBytes(peerData, peerUsername.data(), peerUsernameLen);

            netInterface->SendToClient(from, peerData);
        }

        peers.emplace(newId, from);
        idToUsernameLookup.emplace(newId, username);

        EmitEvent(CLIENT_CONNECTED, newClientData, from);
        SDL_Log("[Server] User %s [%u] connected.", username.c_str(), newId);
    }

    void Server::HandleDisconnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        size_t offset = 1;

        uint32_t clientId = UnpackUint32(data, offset);

        if (peers.contains(clientId))
        {
            std::string username = idToUsernameLookup[clientId];
            peers.erase(clientId);
            idToUsernameLookup.erase(clientId);

            std::vector<uint8_t> acknowledgment;
            acknowledgment.push_back(DISCONNECTION_ACKNOWLEDGED);
            netInterface->SendToClient(from, acknowledgment);

            std::vector<uint8_t> notifyAllData;
            notifyAllData.push_back(CLIENT_DISCONNECTED);
            PackBytes(notifyAllData, &clientId, sizeof(uint32_t));

            for (const auto& client : peers)
                netInterface->SendToClient(client.second, notifyAllData);

            EmitEvent(CLIENT_DISCONNECTED, data, from);
            SDL_Log("[Server] Client Disconnected: %s [%u]", username.c_str(), clientId);
        }
    }

    void Server::HandleUnRequestedConnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {

    }

    void Server::HandleUnRequestedDisconnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        uint32_t clientId = 0;
        for (const auto& client : peers)
        {
            if (from == client.second)
            {
                clientId = client.first;
                break;
            }
        }

        if (clientId == 0 || !peers.contains(clientId))
            return;

        std::string username = idToUsernameLookup[clientId];
        peers.erase(clientId);
        idToUsernameLookup.erase(clientId);

        std::vector<uint8_t> notifyAllData;
        notifyAllData.push_back(CLIENT_DISCONNECTED);
        PackBytes(notifyAllData, &clientId, sizeof(uint32_t));

        for (const auto& client : peers)
            netInterface->SendToClient(client.second, notifyAllData);

        EmitEvent(CLIENT_DISCONNECTED, data, from);
        SDL_Log("[Server] Client Disconnected: %u", clientId);
    }
}
