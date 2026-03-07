#include "interspace/client/ClientWorld.hpp"

#include <ranges>

#include "igneous/input/Input.hpp"
#include "interspace/client/ClientOverworldGenerator.hpp"

#include "interspace/client/ClientTiles.hpp"
#include "interspace/server/ServerTiles.hpp"

namespace Interspace::Client
{
    void ClientWorld::Init()
    {
    }

    void ClientWorld::InitWorld(Client* _client)
    {
        client = _client;

        worldData = std::make_unique<WorldData>();
        RegisterNetEvents();

        tileRegistry = std::make_unique<ClientTiles>();
        tileRegistry->Init();
    }

    void ClientWorld::Update(float delta)
    {
        if (client->clientId <= 0)
            return;

        if (chunkGenerationTimer < chunkGenerationClock)
            chunkGenerationTimer += delta;
        else
        {
            chunkGenerationTimer -= chunkGenerationClock;

            if (worldGenerator)
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

    void ClientWorld::Render()
    {
        for (const auto& faction: factions | std::views::values)
        {
            for (const auto& colonist: faction->colonists | std::views::values)
            {
                colonist->RenderName(faction->name);
            }
        }
    }

    void ClientWorld::HandleEvents(Engine::InputLayer& layer)
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
                ClientFaction* myFaction = nullptr;
                for (const auto& faction: factions | std::views::values)
                {
                    if (faction->ownerId == client->clientId)
                    {
                        myFaction = faction.get();
                        break;
                    }
                }
                if (myFaction)
                {
                    std::string colonistName{"Dwayne"};
                    RequestCreateColonist(myFaction->id, colonistName);
                }
            }
            if (Engine::Input::IsButtonJustPressed(SDL_BUTTON_LEFT))
            {
                ClientFaction* colonistFaction = nullptr;
                ClientColonist* selectedColonist = nullptr;
                bool hasSelectedColonist = false;
                for (const auto& faction: factions | std::views::values)
                {
                    if (!faction->members.contains(client->clientId))
                        continue;

                    for (const auto& colonist: faction->colonists | std::views::values)
                    {
                        if (colonist->sprite->IsMouseWithin())
                        {
                            colonistFaction = faction.get();
                            selectedColonist = colonist.get();
                            break;
                        }
                        if (colonist->selectedBy == client->clientId)
                        {
                            hasSelectedColonist = true;
                        }
                    }
                    if (selectedColonist)
                        break;
                }
                if (selectedColonist && colonistFaction)
                {
                    if (selectedColonist->selectedBy != client->clientId)
                    {
                        RequestColonistSelect(colonistFaction->id, selectedColonist->id);
                    }
                    else
                    {
                        RequestColonistDeselect(colonistFaction->id, selectedColonist->id);
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
                        if (colonist->selectedBy != client->clientId)
                            continue;

                        Engine::Vec2<float> mousePosition = camera->GetMouseGlobalPosition();
                        float posX = mousePosition.x;
                        float posY = mousePosition.y;
                        RequestMoveColonist(faction->id, colonist->id, posX, posY);
                    }
                }
            }
        }
    }

    void ClientWorld::Clean()
    {
    }
}