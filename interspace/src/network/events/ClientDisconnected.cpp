#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/server/World.hpp"

#include <ranges>
#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Server
    {
        void Server::BroadcastDisconnectionToPeers(uint32_t id)
        {
            std::vector<uint8_t> notifyAllData{CLIENT_DISCONNECTED};
            Engine::Serializer notifySerializer(notifyAllData);
            notifySerializer << id;

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
            uint32_t peerId = 0;
            Engine::Deserializer deserializer(data);
            deserializer >> peerId;

            std::string peerUsername = peers[peerId];

            if (peers.contains(peerId))
                peers.erase(peerId);
        }
    }

    namespace Server
    {
        void World::OnClientDisconnected(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            uint32_t clientId = 0;

            Engine::Deserializer deserializer(data);
            deserializer >> clientId;

            BroadcastColonistDeselectAllPacket(clientId);
        }
    }
}