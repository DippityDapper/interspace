#include "interspace/client/World.hpp"

#include <ranges>

#include "igneous/input/Input.hpp"
#include "igneous/networking/Serializer.hpp"

#include "interspace/client/Tiles.hpp"
#include "interspace/network/NetworkPackets.hpp"

namespace Interspace::Client
{
    void World::Init()
    {
    }

    void World::InitWorld(Client* _client)
    {
        client = _client;
        worldGenerator = std::make_unique<WorldGenerator>();

        worldData = std::make_unique<WorldData>();
        RegisterNetEvents();
        Tiles::Init();
    }

    void World::Update(float delta)
    {
        if (client->clientId <= 0)
            return;

        for (const auto& faction: factions | std::views::values)
        {
            faction->Update(delta);
        }

        if (chunkGenerationTimer < chunkGenerationClock)
            chunkGenerationTimer += delta;
        else
        {
            chunkGenerationTimer -= chunkGenerationClock;
            worldGenerator->Tick();
        }

        if (clientTimer < clientClock)
            clientTimer += delta;
        else
        {
            clientTimer -= clientClock;
            SendPosition();
        }
    }

    void World::UI()
    {
        for (const auto& faction: factions | std::views::values)
        {
            for (const auto& colonist: faction->colonists | std::views::values)
            {
                colonist->RenderName(faction->data.name);
            }
        }
    }

    void World::HandleEvents(Engine::InputLayer& layer)
    {
        if (layer.Is("gameplay"))
        {
            if (Engine::Input::IsKeyJustPressed(SDLK_ESCAPE))
            {
                if (!disconnectRequested && client)
                {
                    RequestDisconnect();
                }
            }
            if (Engine::Input::IsKeyJustPressed(SDLK_Q))
            {
                Faction* myFaction = nullptr;
                for (const auto& faction: factions | std::views::values)
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
                    RequestCreateColonist(myFaction->data.id, colonistName);
                }
            }
            if (Engine::Input::IsButtonJustPressed(SDL_BUTTON_LEFT))
            {
                Faction* colonistFaction = nullptr;
                Colonist* selectedColonist = nullptr;
                bool hasSelectedColonist = false;
                for (const auto& faction: factions | std::views::values)
                {
                    if (!faction->data.members.contains(client->clientId))
                        continue;

                    for (const auto& colonist: faction->colonists | std::views::values)
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
                        RequestColonistSelect(colonistFaction->data.id, selectedColonist->entityData.id);
                    }
                    else
                    {
                        RequestColonistDeselect(colonistFaction->data.id, selectedColonist->entityData.id);
                    }
                }
                else if (hasSelectedColonist)
                {
                    RequestColonistDeselectAll();
                }
            }
            if (Engine::Input::IsButtonJustPressed(SDL_BUTTON_RIGHT))
            {
                for (const auto& faction: factions | std::views::values)
                {
                    for (const auto& colonist: faction->colonists | std::views::values)
                    {
                        if (colonist->colonistData.selectedBy != client->clientId)
                            continue;

                        Engine::Vec2<float> mousePosition = camera->GetMouseGlobalPosition();
                        float posX = mousePosition.x;
                        float posY = mousePosition.y;
                        RequestMoveColonist(faction->data.id, colonist->entityData.id, posX, posY);
                    }
                }
            }
        }
    }

    void World::Clean()
    {
    }
}