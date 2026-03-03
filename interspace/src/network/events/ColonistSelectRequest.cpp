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
        void World::RequestColonistSelect(uint16_t factionId, uint32_t colonistId)
        {
            std::vector<uint8_t> data{COLONIST_SELECT_REQUEST};

            Engine::Serializer serializer(data);
            serializer << factionId << colonistId << client->clientId;

            client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Server
    {
        void World::OnColonistSelectRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            uint16_t factionId = 0;
            uint32_t colonistId = 0;
            uint32_t clientId = 0;

            Engine::Deserializer deserializer(data);
            deserializer >> factionId >> colonistId >> clientId;

            if (!factions.contains(factionId))
                return;

            Faction* faction = factions[factionId].get();
            if (!faction->data.members.contains(clientId) || !faction->colonists.contains(colonistId))
                return;

            Colonist* colonist = faction->colonists[colonistId].get();
            colonist->colonistData.selectedBy = clientId;

            BroadcastColonistSelection(factionId, colonistId, clientId);
        }
    }
}