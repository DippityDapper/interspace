#include "SDL3/SDL_log.h"
#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/ClientWorld.hpp"
#include "interspace/game/Game.hpp"

#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Client
    {
        void ClientWorld::RequestDisconnect()
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
            client_id_t clientId = 0;
            Engine::Deserializer deserializer(data);
            deserializer >> clientId;

            if (!Game::server->CheckPeer(clientId, from))
                return;

            if (peers.contains(clientId))
            {
                DisconnectClient(clientId);

                AcknowledgeDisconnection(from);
                BroadcastDisconnectionToPeers(clientId);

                SDL_Log("[Server] Client Disconnected: %s [%u]", GetUsername(clientId).c_str(), clientId);
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
            client_id_t clientId = GetClientId(from);

            if (clientId == 0 || !peers.contains(clientId))
                return;

            DisconnectClient(clientId);
            BroadcastDisconnectionToPeers(clientId);

            SDL_Log("[Server] Client Disconnected: %u", clientId);
            EmitEvent(CLIENT_DISCONNECTED, data, from);
        }
    }
}