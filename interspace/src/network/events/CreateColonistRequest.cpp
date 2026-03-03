#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/World.hpp"
#include "interspace/server/World.hpp"

#include <vector>

namespace Interspace
{
    //----------------------------
    // SEND
    //----------------------------
    namespace Client
    {
        void World::RequestCreateColonist(uint16_t factionId, const std::string& colonistName)
        {
            std::vector<uint8_t> data{CREATE_COLONIST_REQUEST};
            Engine::Serializer serializer(data);
            serializer << factionId << colonistName;
            client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Server
    {
        void World::OnCreateColonistRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from)
        {
            uint16_t factionId = 0;
            std::string colonistName{};

            Engine::Deserializer deserializer(data);
            deserializer >> factionId >> colonistName;

            if (!factions.contains(factionId))
                return;

            AddColonistToFaction(factionId, colonistName);
        }
    }
}