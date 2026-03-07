#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/ClientWorld.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/server/ServerWorld.hpp"

#include <ranges>
#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Client
    {
        void ClientWorld::RequestColonistDeselectAll()
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
        void ServerWorld::OnColonistDeselectAllRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
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