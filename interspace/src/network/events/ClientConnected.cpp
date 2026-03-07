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
        void Server::BroadcastConnectionToPeers(client_id_t clientId, const std::string& username)
        {
            std::vector<uint8_t> newClientData{CLIENT_CONNECTED};
            Engine::Serializer broadcastSerializer(newClientData);

            broadcastSerializer << clientId << username;

            for (const auto& peer: peers)
            {
                netInterface->SendToClient(peer.second, newClientData, ENET_PACKET_FLAG_RELIABLE);
            }
        }

        void Server::SendPeersToPeer(ENetPeer* to)
        {
            for (const auto& clientId: peers | std::views::keys)
            {
                std::vector<uint8_t> peerData{CLIENT_CONNECTED};

                std::string peerUsername = idToUsernameLookup[clientId];

                Engine::Serializer peerSerializer(peerData);
                peerSerializer << clientId << peerUsername;

                netInterface->SendToClient(to, peerData, ENET_PACKET_FLAG_RELIABLE);
            }
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Client
    {
        void Client::OnClientConnected(const std::vector<uint8_t>& data)
        {
            client_id_t clientId = 0;
            std::string peerUsername{};

            Engine::Deserializer deserializer(data);
            deserializer >> clientId >> peerUsername;

            peers[clientId] = peerUsername;
        }
    }

    namespace Server
    {
        void ServerWorld::OnClientConnected(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            client_id_t clientId = 0;
            std::string username{};
            Engine::Deserializer deserializer(data);
            deserializer >> clientId >> username;

            if (!Game::server->CheckPeer(clientId, from))
                return;

            players.emplace(clientId, std::make_unique<Player>());

            SendWorldData(from);
            SendFactionData(from, clientId);

            JoinFaction(clientId);
        }
    }
}