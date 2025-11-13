#include "game/client/WorldClient.hpp"

#include <cstring>

#include "SDL3/SDL_log.h"

#include "dapper2d/Window.hpp"

#include "game/game/Camera.hpp"
#include "game/client/Client.hpp"

namespace Game
{
    WorldClient::WorldClient(Client* _client)
    {
        client = _client;
    }

    void WorldClient::Init()
    {
        client->ConnectToEvent(WORLD_DATA_PACKET, this, &WorldClient::OnWorldDataReceived);
        client->ConnectToEvent(AREA_DATA_PACKET, this, &WorldClient::OnAreaDataReceived);

        client->ConnectToEvent(CONNECTION_ACCEPTED, this, &WorldClient::OnConnectionAccepted);
        client->ConnectToEvent(CLIENT_CONNECTED, this, &WorldClient::OnClientConnected);
        client->ConnectToEvent(CLIENT_DISCONNECTED, this, &WorldClient::OnClientDisconnected);

        RequestWorldData();
    }

    void WorldClient::Update(float delta)
    {
        if (!camera)
            return;
        
        Engine::Vec2<float> cameraPosition = camera->position;
        float minZoom = std::max(camera->zoom, minZoomForRendering);

        Engine::Vec2<float> viewportOffset = ((Engine::Vec2<float>)Engine::Window::viewport  / 2.0f) / minZoom;

        Engine::Vec2<int> minBounds = ((cameraPosition - viewportOffset) / (TILE_SIZE * AREA_SIZE)).Floor();
        Engine::Vec2<int> maxBounds = ((cameraPosition + viewportOffset) / (TILE_SIZE * AREA_SIZE)).Floor();

        int areasAdded = 0;

        for (uint16_t y = minBounds.y-1; y <= maxBounds.y+1; ++y)
        {
            for (uint16_t x = minBounds.x-1; x <= maxBounds.x+1; ++x)
            {
                Engine::Vec2<uint16_t> visibleAreaPosition{x, y};
                if (requestedAreas.contains(visibleAreaPosition))
                    continue;
                if (areas.contains(visibleAreaPosition))
                {
                    AreaClient* area = areas[visibleAreaPosition].get();
                    if (area->generationComplete)
                        area->Update(delta);
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

            if (pos.x < minBounds.x - 1 || pos.x > maxBounds.x + 1 ||
                pos.y < minBounds.y - 1 || pos.y > maxBounds.y + 1)
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

    void WorldClient::Render()
    {
        if (!camera)
            return;

        float zoom = camera->zoom;

        Engine::Vec2<float> cameraPosition = camera->position;
        Engine::Vec2<float> viewportOffset = ((Engine::Vec2<float>)Engine::Window::viewport  / 2.0f) / zoom;

        Engine::Vec2<int> minBounds = ((cameraPosition - viewportOffset) / (TILE_SIZE * AREA_SIZE)).Floor();
        Engine::Vec2<int> maxBounds = ((cameraPosition + viewportOffset) / (TILE_SIZE * AREA_SIZE)).Floor();

        for (uint16_t y = minBounds.y; y <= maxBounds.y; ++y)
        {
            for (uint16_t x = minBounds.x; x <= maxBounds.x; ++x)
            {
                Engine::Vec2<uint16_t> visibleAreaPosition{x, y};

                if (areas.contains(visibleAreaPosition))
                {
                    AreaClient* area = areas[visibleAreaPosition].get();
                    if (area->generationComplete)
                        area->Render();
                }
            }
        }
    }

    void WorldClient::HandleEvents(SDL_Event& event)
    {
    }

    void WorldClient::Clean()
    {
    }

    uint32_t WorldClient::AddFaction(uint32_t ownerId, const std::string& factionName)
    {
        uint32_t factionId = nextFactionId++;
        factions.emplace(factionId, std::make_unique<FactionClient>(factionId, ownerId, factionName));
        factionIdToName.emplace(factionId, factionName);
        factionNameToId.emplace(factionName, factionId);
        factionOwnerToId.emplace(ownerId, factionId);

        SDL_Log("[Client] Faction created: %s [%u]", factionName.c_str(), factionId);

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

    void WorldClient::RequestWorldData()
    {
        std::vector<uint8_t> request;
        request.push_back(WORLD_DATA_REQUEST);
        client->netInterface->SendToServer(request);
    }

    void WorldClient::OnWorldDataReceived(const std::vector<uint8_t>& data)
    {
        size_t offset = 1;

        uint32_t worldNameLen = UnpackUint32(data, offset);
        name = UnpackString(data, offset, worldNameLen);
        worldSizeX = UnpackUint16(data, offset);
        worldSizeY = UnpackUint16(data, offset);

        camera = std::make_unique<Camera>
        (
            (worldSizeX * AREA_SIZE * TILE_SIZE) / 2.0f,
            (worldSizeX * AREA_SIZE * TILE_SIZE) / 2.0f,
            1.0f
        );
        camera->SetCurrent();

        camera->minZoom = 0.05;
        camera->limitBounds = true;
        camera->limitLeft = 0.0f;
        camera->limitRight = worldSizeX * AREA_SIZE * TILE_SIZE;
        camera->limitTop = 0.0f;
        camera->limitBottom = worldSizeY * AREA_SIZE * TILE_SIZE;
    }

    void WorldClient::OnAreaDataReceived(const std::vector<uint8_t>& data)
    {
        size_t offset = 1;
        
        uint16_t areaPositionX = UnpackUint16(data, offset);
        uint16_t areaPositionY = UnpackUint16(data, offset);
        Engine::Vec2<uint16_t> areaPosition = {areaPositionX, areaPositionY};

        if (requestedAreas.contains(areaPosition))
            requestedAreas.erase(areaPosition);

        std::vector<uint8_t> tileData(data.begin()+offset, data.end());

        std::unique_ptr<AreaClient> newArea = std::make_unique<AreaClient>(areaPosition, tileData);
        AreaClient* area = newArea.get();
        areas[areaPosition] = std::move(newArea);

        area->generationComplete = false;

        area->Create();
        area->generationComplete = true;
        area->BakeSprite();
    }

    void WorldClient::OnConnectionAccepted(const std::vector<uint8_t>& data)
    {
        uint32_t id = client->clientId;
        std::string username = client->username;

        AddFaction(id, username);
    }

    void WorldClient::OnClientConnected(const std::vector<uint8_t>& data)
    {
        size_t offset = 1;

        uint32_t peerId = UnpackUint32(data, offset);
        uint32_t usernameLen = UnpackUint32(data, offset);
        std::string peerUsername = UnpackString(data, offset, usernameLen);

        AddFaction(peerId, peerUsername);
    }

    void WorldClient::OnClientDisconnected(const std::vector<uint8_t>& data)
    {
        size_t offset = 1;

        uint32_t peerId = UnpackUint32(data, offset);
        std::string peerUsername = client->GetUsername(peerId);

        RemoveFaction(factionOwnerToId[peerId]);
    }
}
