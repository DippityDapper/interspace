#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/ClientWorld.hpp"
#include "interspace/server/ServerFaction.hpp"
#include "interspace/server/ServerWorld.hpp"

#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Client
    {
        void ClientWorld::RequestMoveColonist(faction_id_t factionId, entity_id_t colonistId, float x, float y)
        {
            std::vector<uint8_t> data{COLONIST_POSITION_REQUEST};
            Engine::Serializer serializer(data);
            serializer << factionId << colonistId << x << y;
            client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Server
    {
        void ServerWorld::OnColonistPositionRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            faction_id_t factionId = 0;
            entity_id_t colonistId = 0;
            float colonistPositionX = 0;
            float colonistPositionY = 0;

            Engine::Deserializer deserializer(data);
            deserializer >> factionId >> colonistId >> colonistPositionX >> colonistPositionY;

            if (!factions.contains(factionId))
                return;

            ServerFaction* faction = factions[factionId].get();
            ServerColonist* colonist = faction->GetColonist(colonistId);

            if (!colonist)
                return;

            colonist->position.x = colonistPositionX;
            colonist->position.y = colonistPositionY;
        }
    }
}