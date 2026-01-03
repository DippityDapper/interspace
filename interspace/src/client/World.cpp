#include "interspace/client/World.hpp"

#include <ranges>

#include "igneous/Camera.hpp"
#include "interspace/menus/CreateFactionMenu.hpp"
#include "interspace/network/Serializer.hpp"
#include "SDL3/SDL_log.h"

namespace Interspace::Client
{
    World::World(Client* _client)
    {
        client = _client;
    }

    void World::Init()
    {
        worldData = std::make_unique<WorldData>();
        RegisterNetEvents();
    }

    void World::Update(float delta)
    {
        for (const auto& faction : factions | std::views::values)
        {
            faction->Update(delta);
        }
    }

    void World::Render()
    {
        for (const auto& faction : factions | std::views::values)
        {
            for (const auto& colonist : faction->colonists | std::views::values)
            {
                colonist->RenderName(faction->data.name);
            }
        }
    }

    void World::HandleEvents(SDL_Event& event)
    {
        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            if (event.key.key == SDLK_ESCAPE)
            {
                if (!disconnectRequested && client)
                {
                    std::vector<uint8_t> request{DISCONNECTION_REQUEST};
                    Serializer serializer(request);

                    serializer << client->clientId;

                    client->netInterface->SendToServer(request, ENET_PACKET_FLAG_RELIABLE);
                    disconnectRequested = true;
                }
            }
        }
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                Faction* colonistFaction = nullptr;
                Colonist* selectedColonist = nullptr;
                bool hasSelectedColonist = false;
                for (const auto& faction : factions | std::views::values)
                {
                    if (!faction->data.members.contains(client->clientId))
                        continue;

                    for (const auto& colonist : faction->colonists | std::views::values)
                    {
                        if (colonist->sprite->IsMouseWithin())
                        {
                            colonistFaction = faction.get();
                            selectedColonist = colonist.get();
                            break;
                        }
                        if (colonist->colonistData.selectedBy == client->clientId)
                        {
                            hasSelectedColonist = true;
                        }
                    }
                    if (selectedColonist)
                        break;
                }
                if (selectedColonist && colonistFaction)
                {
                    if (selectedColonist->colonistData.selectedBy != client->clientId)
                    {
                        std::vector<uint8_t> data{COLONIST_SELECT_REQUEST};

                        Serializer serializer(data);
                        serializer
                            << colonistFaction->data.id
                            << selectedColonist->entityData.id
                            << client->clientId;

                        client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
                    }
                    else
                    {
                        std::vector<uint8_t> data{COLONIST_DESELECT_REQUEST};

                        Serializer serializer(data);
                        serializer
                            << colonistFaction->data.id
                            << selectedColonist->entityData.id
                            << client->clientId;

                        client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
                    }
                }
                else if (hasSelectedColonist)
                {
                    std::vector<uint8_t> data{COLONIST_DESELECT_ALL_REQUEST};

                    Serializer serializer(data);
                    serializer << client->clientId;
                    client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
                }
            }
            if (event.button.button == SDL_BUTTON_RIGHT)
            {
                for (const auto& faction : factions | std::views::values)
                {
                    for (const auto& colonist : faction->colonists | std::views::values)
                    {
                        if (colonist->colonistData.selectedBy != client->clientId)
                            continue;

                        std::vector<uint8_t> data{COLONIST_POSITION_REQUEST};

                        Engine::Vec2<float> mousePosition = camera->GetMouseGlobalPosition();
                        float posX = mousePosition.x;
                        float posY = mousePosition.y;

                        Serializer serializer(data);
                        serializer
                            << faction->data.id
                            << colonist->entityData.id
                            << posX
                            << posY;

                        client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
                    }
                }
            }
        }
        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            if (event.key.key == SDLK_Q)
            {
                Faction* myFaction = nullptr;
                for (const auto& faction : factions | std::views::values)
                {
                    if (faction->data.ownerId == client->clientId)
                    {
                        myFaction = faction.get();
                        break;
                    }
                }
                if (myFaction)
                {
                    std::string colonistName{"Dwayne"};
                    std::vector<uint8_t> data{CREATE_COLONIST_REQUEST};

                    Serializer serializer(data);
                    serializer << myFaction->data.id << colonistName;

                    client->netInterface->SendToServer(data, ENET_PACKET_FLAG_RELIABLE);
                }
            }
        }
    }

    void World::Clean()
    {
    }
}
