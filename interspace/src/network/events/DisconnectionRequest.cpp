#include "SDL3/SDL_log.h"
#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/World.hpp"

#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Client
    {
        void World::RequestDisconnect()
        {
            std::vector<uint8_t> request{DISCONNECTION_REQUEST};
            Engine::Serializer serializer(request);

            serializer << client->clientId;

            client->netInterface->SendToServer(request, ENET_PACKET_FLAG_RELIABLE);
            disconnectRequested = true;
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Server
    {
        void Server::OnDisconnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            uint32_t clientId = 0;
            Engine::Deserializer deserializer(data);
            deserializer >> clientId;

            if (peers.contains(clientId))
            {
                std::string username = idToUsernameLookup[clientId];
                peers.erase(clientId);
                idToUsernameLookup.erase(clientId);

                AcknowledgeDisconnection(from);
                BroadcastDisconnectionToPeers(clientId);

                SDL_Log("[Server] Client Disconnected: %s [%u]", username.c_str(), clientId);
                EmitEvent(CLIENT_DISCONNECTED, data, from);
            }
        }
    }

    //----------------------------
    // UNREQUESTED
    //----------------------------
    namespace Server
    {
        void Server::OnUnRequestedDisconnectionRequest(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            uint32_t clientId = 0;
            for (const auto& client: peers)
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
            Engine::Serializer notifySerializer(notifyAllData);
            notifySerializer << clientId;

            for (const auto& client: peers)
                netInterface->SendToClient(client.second, notifyAllData, ENET_PACKET_FLAG_RELIABLE);

            SDL_Log("[Server] Client Disconnected: %u", clientId);
            EmitEvent(CLIENT_DISCONNECTED, data, from);
        }
    }
}