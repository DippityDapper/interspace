#include "game/network/Server.hpp"

#include "SDL3/SDL_log.h"

#include "game/network/NetworkPackets.hpp"

namespace Game
{
    void Server::HandleConnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        size_t offset = 1; // start after the request

        uint32_t usernameLength;
        memcpy(&usernameLength, &data[offset], sizeof(uint32_t));
        offset += sizeof(uint32_t);

        std::string username(reinterpret_cast<const char*>(&data[offset]), usernameLength);

        uint32_t newId = nextPeerId++;

        SDL_Log("[Server] User %s [%u] connected.", username.c_str(), newId);

        std::vector<uint8_t> response;
        response.push_back(CONNECTION_ACCEPTED);

        uint8_t* idBytes = reinterpret_cast<uint8_t*>(&newId);
        response.insert(response.end(), idBytes, idBytes + sizeof(uint32_t));

        netInterface->SendToClient(from, response);

        std::vector<uint8_t> responseAll;
        responseAll.push_back(CLIENT_CONNECTED);

        uint8_t* newClientIdBytes = reinterpret_cast<uint8_t*>(&newId);
        responseAll.insert(responseAll.end(), newClientIdBytes, newClientIdBytes + sizeof(uint32_t));

        uint32_t usernameLen = username.size();
        uint8_t* usernameBytes = reinterpret_cast<uint8_t*>(&usernameLen);
        responseAll.insert(responseAll.end(), usernameBytes, usernameBytes + sizeof(uint32_t));
        responseAll.insert(responseAll.end(), username.begin(), username.end());

        for (const auto& peer : peers)
        {
            netInterface->SendToClient(peer.second, responseAll);

            std::vector<uint8_t> responseOfPeers;
            responseOfPeers.push_back(CLIENT_CONNECTED);

            uint32_t peerId = peer.first;
            uint8_t* peerIdBytes = reinterpret_cast<uint8_t*>(&peerId);
            responseOfPeers.insert(responseOfPeers.end(), peerIdBytes, peerIdBytes + sizeof(uint32_t));

            std::string peerUsername = idToUsernameLookup[peerId];
            uint32_t peerUsernameLen = peerUsername.size();
            uint8_t* peerUsernameLenBytes = reinterpret_cast<uint8_t*>(&peerUsernameLen);
            responseOfPeers.insert(responseOfPeers.end(), peerUsernameLenBytes, peerUsernameLenBytes + sizeof(uint32_t));
            responseOfPeers.insert(responseOfPeers.end(), peerUsername.begin(), peerUsername.end());

            netInterface->SendToClient(from, responseOfPeers);
        }

        peers.emplace(newId, from);
        idToUsernameLookup.emplace(newId, username);

        EmitEvent(CLIENT_CONNECTED, data, from);
    }

    void Server::HandleDisconnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        size_t offset = 1;

        uint32_t clientId;
        memcpy(&clientId, &data[offset], sizeof(uint32_t));

        if (peers.contains(clientId))
        {
            std::string username = idToUsernameLookup[clientId];
            peers.erase(clientId);
            idToUsernameLookup.erase(clientId);

            SDL_Log("[Server] Client Disconnected: %s [%u]", username.c_str(), clientId);

            std::vector<uint8_t> responseBack;
            responseBack.push_back(DISCONNECTION_ACKNOWLEDGED);
            netInterface->SendToClient(from, responseBack);

            std::vector<uint8_t> responseAll;
            responseAll.push_back(CLIENT_DISCONNECTED);
            uint8_t* clientIdBytes = reinterpret_cast<uint8_t*>(&clientId);
            responseAll.insert(responseAll.end(), clientIdBytes, clientIdBytes + sizeof(uint32_t));

            for (const auto& client : peers)
                netInterface->SendToClient(client.second, responseAll);

            EmitEvent(CLIENT_DISCONNECTED, data, from);
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

        SDL_Log("[Server] Client Disconnected: %u", clientId);

        std::vector<uint8_t> responseAll;
        responseAll.push_back(CLIENT_DISCONNECTED);
        uint8_t* clientIdBytes = reinterpret_cast<uint8_t*>(&clientId);
        responseAll.insert(responseAll.end(), clientIdBytes, clientIdBytes + sizeof(uint32_t));

        for (const auto& client : peers)
            netInterface->SendToClient(client.second, responseAll);

        EmitEvent(CLIENT_DISCONNECTED, data, from);
    }
}
