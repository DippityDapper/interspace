#include "game/client/WorldClient.hpp"

#include <cstring>

#include "imgui.h"
#include "SDL3/SDL_log.h"

#include "dapper2d/Window.hpp"

#include "game/game/Camera.hpp"
#include "game/client/Client.hpp"
#include "SDL3/SDL_events.h"

namespace Game
{
    WorldClient::WorldClient(Client* _client)
    {
        client = _client;
    }

    void WorldClient::Init()
    {
        ConnectNetEvents();

        std::vector<uint8_t> request;
        request.push_back(WORLD_DATA_REQUEST);
        client->netInterface->SendToServer(request);
    }

    void WorldClient::Update(float delta)
    {
        if (sendTimer < sendClock)
        {
            sendTimer += delta;
        }
        else
        {
            sendTimer = 0.0f;
            SendPlayerPosition();
        }

        UpdateAreas(delta);
    }

    void WorldClient::Render()
    {
        if (!camera)
            return;

        RenderVisibleAreas();
        RenderFactions();
        DisplayPlayerNames();
    }

    void WorldClient::HandleEvents(SDL_Event& event)
    {
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            if (event.button.button == SDL_BUTTON_RIGHT)
            {
                for (const auto& kvp : players[client->clientId]->selectedColonists)
                {
                    std::vector<uint8_t> request;
                    request.push_back(COLONIST_POSITION_REQUEST);

                    float mouseScreenX = event.button.x;
                    float mouseScreenY = event.button.y;

                    float cameraX = camera->position.x;
                    float cameraY = camera->position.y;

                    Engine::Vec2<int> viewport = Engine::Window::viewport;
                    float viewportCenterX = viewport.x * 0.5f;
                    float viewportCenterY = viewport.y * 0.5f;

                    float mouseGlobalX = cameraX + (mouseScreenX - viewportCenterX) / camera->zoom;
                    float mouseGlobalY = cameraY + (mouseScreenY - viewportCenterY) / camera->zoom;

                    PackBytes(request, &client->clientId, sizeof(uint32_t));
                    PackBytes(request, &kvp.first, sizeof(uint32_t));
                    PackBytes(request, &mouseGlobalX, sizeof(float));
                    PackBytes(request, &mouseGlobalY, sizeof(float));

                    client->netInterface->SendToServer(request);
                }
            }
        }
    }

    void WorldClient::Clean()
    {
    }

    bool WorldClient::AddPlayer(uint32_t clientId, const Engine::Vec2<uint64_t>& position)
    {
        if (players.contains(clientId))
            return false;
        players.emplace(clientId, std::make_unique<PlayerClient>(clientId, position));
        return true;
    }

    bool WorldClient::RemovePlayer(uint32_t clientId)
    {
        if (!players.contains(clientId))
            return false;
        players.erase(clientId);
        return true;
    }

    uint32_t WorldClient::AddFaction(uint32_t ownerId, const std::string& factionName)
    {
        uint32_t factionId = nextFactionId++;
        factions.emplace(factionId, std::make_unique<FactionClient>(factionId, ownerId, factionName));
        factionIdToName.emplace(factionId, factionName);
        factionNameToId.emplace(factionName, factionId);
        factionOwnerToId.emplace(ownerId, factionId);

        // SDL_Log("[Client] Faction created: %s [%u]", factionName.c_str(), factionId);
        for (const auto& faction : factions)
        {
            for (const auto& colonist : faction.second->colonists)
            {
                players[ownerId]->selectedColonists.emplace(colonist.first, colonist.second.get());
            }
        }

        return factionId;
    }

    uint32_t WorldClient::AddFaction(uint32_t factionId, uint32_t ownerId, const std::string& factionName)
    {
        if (factions.contains(factionId))
            return 0;

        factions.emplace(factionId, std::make_unique<FactionClient>(factionId, ownerId, factionName));
        factionIdToName.emplace(factionId, factionName);
        factionNameToId.emplace(factionName, factionId);
        factionOwnerToId.emplace(ownerId, factionId);

        // SDL_Log("[Client] Faction created: %s [%u]", factionName.c_str(), factionId);
        for (const auto& kvp : factions[factionId]->colonists)
        {
            players[ownerId]->selectedColonists.emplace(kvp.first, kvp.second.get());
        }

        return factionId;
    }

    bool WorldClient::RemoveFaction(uint32_t factionId)
    {
        if (!factions.contains(factionId))
            return false;

        factions.erase(factionId);

        if (!factionIdToName.contains(factionId))
            return false;

        std::string factionName = factionIdToName[factionId];
        if (!factionNameToId.contains(factionName))
        {
            factionIdToName.erase(factionId);
            return false;
        }

        uint32_t ownerId = factionNameToId[factionName];
        if (!factionOwnerToId.contains(ownerId))
        {
            factionNameToId.erase(factionName);
            factionIdToName.erase(factionId);
            return false;
        }

        factionOwnerToId.erase(ownerId);
        factionNameToId.erase(factionName);
        factionIdToName.erase(factionId);
        return true;
    }

    void WorldClient::SendPlayerPosition()
    {
        if (!camera)
            return;

        Engine::Vec2<float> cameraPosition = camera->targetPosition;

        if (std::abs(previousCameraPosition.x - cameraPosition.x) > 3 || std::abs(previousCameraPosition.y - cameraPosition.y) > 3)
        {
            previousCameraPosition.x = cameraPosition.x;
            previousCameraPosition.y = cameraPosition.y;

            std::vector<uint8_t> positionData{};
            positionData.push_back(POSITION_PACKET);
            PackBytes(positionData, &client->clientId, sizeof(uint32_t));
            PackBytes(positionData, &previousCameraPosition.x, sizeof(uint64_t));
            PackBytes(positionData, &previousCameraPosition.y, sizeof(uint64_t));

            client->netInterface->SendToServer(positionData);
        }
    }

    void WorldClient::UpdateAreas(float delta)
    {
        if (!camera)
            return;

        Engine::Vec2<float> cameraPosition = camera->position;
        Engine::Vec2<int> renderDist{renderDistance - 1, renderDistance - 1};

        Engine::Vec2<int> minBounds = (cameraPosition / (TILE_SIZE * AREA_SIZE)).Floor() - renderDist;
        Engine::Vec2<int> maxBounds = (cameraPosition / (TILE_SIZE * AREA_SIZE)).Floor() + renderDist;

        int areasAdded = 0;

        for (uint16_t y = minBounds.y; y <= maxBounds.y; ++y)
        {
            for (uint16_t x = minBounds.x; x <= maxBounds.x; ++x)
            {
                Engine::Vec2<uint16_t> visibleAreaPosition{x, y};
                if (requestedAreas.contains(visibleAreaPosition))
                    continue;

                if (areas.contains(visibleAreaPosition))
                {
                    areas[visibleAreaPosition]->Update(delta);
                }
                else if (areasAdded <= 1 && !areas.contains(visibleAreaPosition) && x >= 0 && x < worldSizeX && y >= 0 && y < worldSizeY)
                {
                    std::vector<uint8_t> request;
                    request.push_back(AREA_DATA_REQUEST);

                    uint8_t* areaPositionXBytes = reinterpret_cast<uint8_t*>(&visibleAreaPosition.x);
                    request.insert(request.end(), areaPositionXBytes, areaPositionXBytes + sizeof(uint32_t));

                    uint8_t* areaPositionYBytes = reinterpret_cast<uint8_t*>(&visibleAreaPosition.y);
                    request.insert(request.end(), areaPositionYBytes, areaPositionYBytes + sizeof(uint32_t));

                    client->netInterface->SendToServer(request);
                    requestedAreas[visibleAreaPosition] = true;
                    areasAdded++;
                }
            }
        }

        for (auto it = areas.begin(); it != areas.end();)
        {
            const Engine::Vec2<uint16_t>& pos = it->first;

            if (pos.x < minBounds.x || pos.x > maxBounds.x ||
                pos.y < minBounds.y || pos.y > maxBounds.y)
            {
                if (it->second->generationComplete)
                    it = areas.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void WorldClient::RenderVisibleAreas()
    {
        for (const auto& kvp : areas)
        {
            kvp.second->Render();
        }
    }

    void WorldClient::DisplayPlayerNames()
    {
        for (const auto& kvp : players)
        {
            if (client->GetUsername(kvp.first).empty())
                continue;

            PlayerClient* player = kvp.second.get();
            Engine::Vec2 playerPosition = player->position;
            std::string playerUsername = client->GetUsername(player->clientId);
            Engine::Vec2<int> viewport = Engine::Window::viewport;

            float camPosX = camera->position.x;
            float camPosY = camera->position.y;
            float peerPosX = playerPosition.x;
            float peerPosY = playerPosition.y;

            float screenX = (peerPosX - camPosX) * camera->zoom + viewport.x * 0.5f ;
            float screenY = (peerPosY - camPosY) * camera->zoom + viewport.y * 0.5f;

            if (screenX < 0 || screenX > viewport.x || screenY < 0 || screenY > viewport.y)
                continue;

            ImGui::SetNextWindowPos({screenX, screenY}, 0, {0.5f, 0.5f});
            ImGui::Begin(playerUsername.c_str(), nullptr,
                ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoInputs |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoDecoration
            );
            ImGui::Text(playerUsername.c_str());
            ImGui::End();
        }
    }

    void WorldClient::RenderFactions()
    {
        for (const auto& faction : factions)
        {
            for (const auto& colonist : faction.second->GetColonists())
            {
                uint16_t colonistAreaPositionX = colonist.second->position.x / (TILE_SIZE * AREA_SIZE);
                uint16_t colonistAreaPositionY = colonist.second->position.y / (TILE_SIZE * AREA_SIZE);
                Engine::Vec2<uint16_t> colonistAreaPosition(colonistAreaPositionX, colonistAreaPositionY);

                if (!areas.contains(colonistAreaPosition))
                    continue;

                colonist.second->Render();
            }
        }
    }
}
