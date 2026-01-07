#include <random>

#include "igneous/Database.hpp"
#include "interspace/game/Game.hpp"
#include "SDL3/SDL_log.h"
#include "interspace/network/Serializer.hpp"
#include "interspace/game/DBHelper.hpp"

namespace Interspace::Server
{
    void Server::HandleConnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        std::string username{};
        Deserializer deserializer(data);
        deserializer >> username;

        if (!DBHelper::PlayerExistsByName(username))
        {
            std::mt19937 gen(std::random_device{}());

            int peerId = 0;
            do
            {
                std::uniform_int_distribution<> peerIdDist(1, INT32_MAX);
                peerId = peerIdDist(gen);
            } while (DBHelper::PlayerExists(peerId));

            DBHelper::InsertPlayer(peerId, username);
        }

        uint32_t newId = DBHelper::GetPlayerIdByName(username);

        std::vector<uint8_t> response{CONNECTION_ACCEPTED};
        Serializer responseSerializer(response);
        responseSerializer << newId;

        netInterface->SendToClient(from, response, ENET_PACKET_FLAG_RELIABLE);

        std::vector<uint8_t> newClientData{CLIENT_CONNECTED};
        Serializer broadcastSerializer(newClientData);

        broadcastSerializer
            << newId
            << username;

        for (const auto& peer : peers)
        {
            netInterface->SendToClient(peer.second, newClientData, ENET_PACKET_FLAG_RELIABLE);

            std::vector<uint8_t> peerData{CLIENT_CONNECTED};

            uint32_t peerId = peer.first;
            std::string peerUsername = idToUsernameLookup[peerId];

            Serializer peerSerializer(peerData);
            peerSerializer
                << peerId
                << peerUsername;

            netInterface->SendToClient(from, peerData, ENET_PACKET_FLAG_RELIABLE);
        }

        peers.emplace(newId, from);
        idToUsernameLookup.emplace(newId, username);

        SDL_Log("[Server] User %s [%u] connected.", username.c_str(), newId);
        EmitEvent(CLIENT_CONNECTED, newClientData, from);
    }

    void Server::HandleDisconnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        uint32_t clientId = 0;
        Deserializer deserializer(data);
        deserializer >> clientId;

        if (peers.contains(clientId))
        {
            std::string username = idToUsernameLookup[clientId];
            peers.erase(clientId);
            idToUsernameLookup.erase(clientId);

            std::vector<uint8_t> acknowledgment{DISCONNECTION_ACKNOWLEDGED};
            netInterface->SendToClient(from, acknowledgment, ENET_PACKET_FLAG_RELIABLE);

            std::vector<uint8_t> notifyAllData{CLIENT_DISCONNECTED};
            Serializer notifySerializer(notifyAllData);
            notifySerializer << clientId;

            for (const auto& client : peers)
                netInterface->SendToClient(client.second, notifyAllData, ENET_PACKET_FLAG_RELIABLE);

            SDL_Log("[Server] Client Disconnected: %s [%u]", username.c_str(), clientId);
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

        std::vector<uint8_t> notifyAllData{CLIENT_DISCONNECTED};
        Serializer notifySerializer(notifyAllData);
        notifySerializer << clientId;

        for (const auto& client : peers)
            netInterface->SendToClient(client.second, notifyAllData, ENET_PACKET_FLAG_RELIABLE);

        SDL_Log("[Server] Client Disconnected: %u", clientId);
        EmitEvent(CLIENT_DISCONNECTED, data, from);
    }
}
