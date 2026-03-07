#include "enet/enet.h"
#include "igneous/networking/Serializer.hpp"
#include "igneous/scenes/SceneManager.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/client/ClientWorld.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/menus/CreateFactionMenu.hpp"
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
        void ServerWorld::SendFactionData(ENetPeer* to, client_id_t clientId)
        {
            std::vector<uint8_t> data{FACTION_DATA_PACKET};
            Engine::Serializer serializer(data);

            int32_t factionsCount = factions.size();
            serializer << factionsCount;

            for (const auto& faction: factions | std::views::values)
            {
                uint32_t membersCount = faction->members.size();

                serializer << faction->id << faction->name << faction->ownerId << membersCount;

                for (const auto& memberId: faction->members | std::views::keys)
                {
                    serializer << memberId;
                }

                uint32_t colonistCount = faction->colonists.size();
                serializer << colonistCount;

                for (const auto& kvp: faction->colonists)
                {
                    entity_id_t colonistId = kvp.first;
                    ServerColonist* colonist = kvp.second.get();

                    serializer
                            << colonistId
                            << colonist->name
                            << colonist->position.x
                            << colonist->position.y
                            << colonist->selectedBy;
                }
            }

            server->netInterface->SendToClient(to, data, ENET_PACKET_FLAG_RELIABLE);
        }

        void ServerWorld::BroadcastFactionData(faction_id_t factionId)
        {
            std::vector<uint8_t> data{FACTION_DATA_PACKET};
            Engine::Serializer serializer(data);

            if (!factions.contains(factionId))
                return;

            uint32_t factionsCount = 1;
            serializer << factionsCount;

            ServerFaction* faction = factions[factionId].get();

            uint32_t membersCount = faction->members.size();

            serializer << faction->id << faction->name << faction->ownerId << membersCount;

            for (const auto& memberId: faction->members | std::views::keys)
            {
                serializer << memberId;
            }

            uint32_t colonistCount = faction->colonists.size();
            serializer << colonistCount;

            for (const auto& kvp: faction->colonists)
            {
                entity_id_t colonistId = kvp.first;
                ServerColonist* colonist = kvp.second.get();

                serializer
                        << colonistId
                        << colonist->name
                        << colonist->position.x
                        << colonist->position.y
                        << colonist->selectedBy;
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
        void ClientWorld::OnFactionDataReceived(const std::vector<uint8_t>& data)
        {
            Engine::Deserializer deserializer(data);

            uint32_t factionCount = 0;
            deserializer >> factionCount;

            for (uint32_t i = 0; i < factionCount; i++)
            {
                faction_id_t factionId = 0;
                deserializer >> factionId;

                if (!factions.contains(factionId))
                    factions.emplace(factionId, std::make_unique<ClientFaction>());

                ClientFaction* faction = factions[factionId].get();
                faction->id = factionId;

                deserializer >> faction->name >> faction->ownerId;

                uint32_t membersCount = 0;
                deserializer >> membersCount;

                for (uint32_t j = 0; j < membersCount; j++)
                {
                    client_id_t memberId;
                    deserializer >> memberId;
                    if (!faction->members.contains(memberId))
                    {
                        std::string memberUsername = client->GetUsername(memberId);
                        faction->members.emplace(memberId, memberUsername);
                    }
                }

                uint32_t colonistCount = 0;
                deserializer >> colonistCount;

                for (uint32_t j = 0; j < colonistCount; j++)
                {
                    entity_id_t colonistId = 0;
                    deserializer >> colonistId;

                    if (!faction->colonists.contains(colonistId))
                        faction->colonists.emplace(colonistId, std::make_unique<ClientColonist>());

                    ClientColonist* colonist = faction->colonists[colonistId].get();
                    colonist->id = colonistId;

                    deserializer >> colonist->name >> colonist->position.x >> colonist->position.y >> colonist->selectedBy;

                    if (colonist->selectedBy == client->clientId)
                        colonist->sprite->SetTexture("assets/colonists/colonist_green_spritesheet.png");
                    else if (colonist->selectedBy == 0)
                        colonist->sprite->SetTexture("assets/colonists/colonist_blue_spritesheet.png");
                    else
                        colonist->sprite->SetTexture("assets/colonists/colonist_pink_spritesheet.png");

                    if (faction->members.contains(client->clientId) && faction->colonists.size() == 1)
                    {
                        camera->targetPosition.x = colonist->position.x;
                        camera->targetPosition.y = colonist->position.y;
                        camera->position.x = colonist->position.x;
                        camera->position.y = colonist->position.y;
                    }
                }
            }
        }
    }
}