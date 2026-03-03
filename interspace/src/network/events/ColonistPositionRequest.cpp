#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/World.hpp"
#include "interspace/server/Faction.hpp"
#include "interspace/server/World.hpp"

#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Client
    {
        void World::RequestMoveColonist(uint16_t factionId, uint32_t colonistId, float x, float y)
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
        void World::OnColonistPositionRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            uint16_t factionId = 0;
            uint32_t colonistId = 0;
            float colonistPositionX = 0;
            float colonistPositionY = 0;

            Engine::Deserializer deserializer(data);
            deserializer >> factionId >> colonistId >> colonistPositionX >> colonistPositionY;

            if (!factions.contains(factionId))
                return;

            Faction* faction = factions[factionId].get();
            Colonist* colonist = faction->GetColonist(colonistId);

            if (!colonist)
                return;

            colonist->entityData.position.x = colonistPositionX;
            colonist->entityData.position.y = colonistPositionY;
        }
    }
}