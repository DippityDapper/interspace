#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/World.hpp"
#include "interspace/server/World.hpp"

#include <ranges>
#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Client
    {
        void World::RequestColonistDeselectAll()
        {
            std::vector<uint8_t> data{COLONIST_DESELECT_ALL_REQUEST};

            Engine::Serializer serializer(data);
            serializer << client->clientId;
            client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Server
    {
        void World::OnColonistDeselectAllRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            uint32_t clientId = 0;

            Engine::Deserializer deserializer(data);
            deserializer >> clientId;

            BroadcastColonistDeselectAllPacket(clientId);
        }
    }
}