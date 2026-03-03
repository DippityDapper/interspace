#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/server/World.hpp"

#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Server
    {
        void Server::BroadcastConnectionToPeers(uint32_t id, const std::string& username)
        {
            std::vector<uint8_t> newClientData{CLIENT_CONNECTED};
            Engine::Serializer broadcastSerializer(newClientData);

            broadcastSerializer << id << username;

            for (const auto& peer: peers)
            {
                netInterface->SendToClient(peer.second, newClientData, ENET_PACKET_FLAG_RELIABLE);
            }
        }

        void Server::SendPeersToPeer(ENetPeer* to)
        {
            for (const auto& peer: peers)
            {
                std::vector<uint8_t> peerData{CLIENT_CONNECTED};

                uint32_t peerId = peer.first;
                std::string peerUsername = idToUsernameLookup[peerId];

                Engine::Serializer peerSerializer(peerData);
                peerSerializer
                        << peerId
                        << peerUsername;

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
            uint32_t peerId = 0;
            std::string peerUsername{};

            Engine::Deserializer deserializer(data);
            deserializer >> peerId >> peerUsername;

            peers[peerId] = peerUsername;
        }
    }

    namespace Server
    {
        void World::OnClientConnected(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            uint32_t clientId = 0;
            std::string username{};
            Engine::Deserializer deserializer(data);
            deserializer >> clientId >> username;

            players.emplace(clientId, std::make_unique<Player>());

            SendWorldData(from);
            SendFactionData(from, clientId);

            JoinFaction(clientId);
        }
    }
}