#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "igneous/scenes/SceneManager.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/World.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/menus/CreateFactionMenu.hpp"
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
        void World::SendFactionData(ENetPeer* to, uint32_t clientId)
        {
            std::vector<uint8_t> data{FACTION_DATA_PACKET};
            Engine::Serializer serializer(data);

            int32_t factionsCount = factions.size();
            serializer << factionsCount;

            for (const auto& faction: factions | std::views::values)
            {
                uint32_t membersCount = faction->data.members.size();

                serializer
                        << faction->data.id
                        << faction->data.name
                        << faction->data.ownerId
                        << membersCount;

                for (const auto& memberId: faction->data.members | std::views::keys)
                {
                    serializer << memberId;
                }

                uint32_t colonistCount = faction->colonists.size();
                serializer << colonistCount;

                for (const auto& kvp: faction->colonists)
                {
                    uint32_t colonistId = kvp.first;
                    Colonist* colonist = kvp.second.get();

                    serializer
                            << colonistId
                            << colonist->entityData.name
                            << colonist->entityData.position.x
                            << colonist->entityData.position.y
                            << colonist->colonistData.selectedBy;
                }
            }

            server->netInterface->SendToClient(to, data, ENET_PACKET_FLAG_RELIABLE);
        }

        void World::BroadcastFactionData(uint16_t factionId)
        {
            std::vector<uint8_t> data{FACTION_DATA_PACKET};
            Engine::Serializer serializer(data);

            if (!factions.contains(factionId))
                return;

            uint32_t factionsCount = 1;
            serializer << factionsCount;

            Faction* faction = factions[factionId].get();

            uint32_t membersCount = faction->data.members.size();

            serializer
                    << faction->data.id
                    << faction->data.name
                    << faction->data.ownerId
                    << membersCount;

            for (const auto& memberId: faction->data.members | std::views::keys)
            {
                serializer << memberId;
            }

            uint32_t colonistCount = faction->colonists.size();
            serializer << colonistCount;

            for (const auto& kvp: faction->colonists)
            {
                uint32_t colonistId = kvp.first;
                Colonist* colonist = kvp.second.get();

                serializer
                        << colonistId
                        << colonist->entityData.name
                        << colonist->entityData.position.x
                        << colonist->entityData.position.y
                        << colonist->colonistData.selectedBy;
            }

            for (const auto& peer: server->GetPeers() | std::views::values)
                server->netInterface->SendToClient(peer, data, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    //----------------------------
    // RECEIVE
    //----------------------------
    namespace Client
    {
        void World::OnFactionDataReceived(const std::vector<uint8_t>& data)
        {
            Engine::Deserializer deserializer(data);

            uint32_t factionCount = 0;
            deserializer >> factionCount;

            for (uint32_t i = 0; i < factionCount; i++)
            {
                uint16_t factionId = 0;
                deserializer >> factionId;

                if (!factions.contains(factionId))
                    factions.emplace(factionId, std::make_unique<Faction>());

                Faction* faction = factions[factionId].get();
                faction->data.id = factionId;

                deserializer >> faction->data.name >> faction->data.ownerId;

                uint32_t membersCount = 0;
                deserializer >> membersCount;

                for (uint32_t j = 0; j < membersCount; j++)
                {
                    uint32_t memberId;
                    deserializer >> memberId;
                    if (!faction->data.members.contains(memberId))
                    {
                        std::string memberUsername = client->GetUsername(memberId);
                        faction->data.members.emplace(memberId, memberUsername);
                    }
                }

                uint32_t colonistCount = 0;
                deserializer >> colonistCount;

                for (uint32_t j = 0; j < colonistCount; j++)
                {
                    uint32_t colonistId = 0;
                    deserializer >> colonistId;

                    if (!faction->colonists.contains(colonistId))
                        faction->colonists.emplace(colonistId, std::make_unique<Colonist>());

                    Colonist* colonist = faction->colonists[colonistId].get();
                    colonist->entityData.id = colonistId;

                    deserializer >> colonist->entityData.name >> colonist->entityData.position.x >> colonist->entityData.position.y >> colonist->colonistData.selectedBy;

                    if (colonist->colonistData.selectedBy == client->clientId)
                        colonist->sprite->SetTexture("assets/colonists/colonist_green_spritesheet.png");
                    else if (colonist->colonistData.selectedBy == 0)
                        colonist->sprite->SetTexture("assets/colonists/colonist_blue_spritesheet.png");
                    else
                        colonist->sprite->SetTexture("assets/colonists/colonist_pink_spritesheet.png");

                    if (faction->data.members.contains(client->clientId) && faction->colonists.size() == 1)
                    {
                        camera->targetPosition.x = colonist->entityData.position.x;
                        camera->targetPosition.y = colonist->entityData.position.y;
                        camera->position.x = colonist->entityData.position.x;
                        camera->position.y = colonist->entityData.position.y;
                    }
                }
            }
        }
    }
}