#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/server/ServerWorld.hpp"

#include <ranges>
#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Server
    {
        void Server::BroadcastDisconnectionToPeers(client_id_t clientId)
        {
            std::vector<uint8_t> notifyAllData{CLIENT_DISCONNECTED};
            Engine::Serializer notifySerializer(notifyAllData);
            notifySerializer << clientId;

            for (const auto& peer: peers | std::views::values)
                netInterface->SendToClient(peer, notifyAllData, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Client
    {
        void Client::OnClientDisconnected(const std::vector<uint8_t>& data)
        {
            client_id_t clientId = 0;
            Engine::Deserializer deserializer(data);
            deserializer >> clientId;

            std::string peerUsername = peers[clientId];

            if (peers.contains(clientId))
                peers.erase(clientId);
        }
    }

    namespace Server
    {
        void ServerWorld::OnClientDisconnected(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            client_id_t clientId = 0;

            Engine::Deserializer deserializer(data);
            deserializer >> clientId;

            if (!Game::server->CheckPeer(clientId, from))
                return;

            BroadcastColonistDeselectAllPacket(clientId);
        }
    }
}