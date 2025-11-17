#include "game/client/WorldClient.hpp"

#include "game/client/Client.hpp"
#include "game/network/NetworkPackets.hpp"

namespace Game
{
    void WorldClient::ConnectNetEvents()
    {
        client->ConnectToEvent(WORLD_DATA_PACKET, this, &WorldClient::OnWorldDataReceived);
        client->ConnectToEvent(AREA_DATA_PACKET, this, &WorldClient::OnAreaDataReceived);
        client->ConnectToEvent(FACTION_DATA_PACKET, this, &WorldClient::OnFactionDataReceived);

        client->ConnectToEvent(CONNECTION_ACCEPTED, this, &WorldClient::OnConnectionAccepted);
        client->ConnectToEvent(CLIENT_CONNECTED, this, &WorldClient::OnClientConnected);
        client->ConnectToEvent(CLIENT_DISCONNECTED, this, &WorldClient::OnClientDisconnected);

        client->ConnectToEvent(POSITION_PACKET, this, &WorldClient::OnPositionDataReceived);

        client->ConnectToEvent(COLONIST_POSITION_PACKET, this, &WorldClient::OnColonistPositionDataReceived);
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

        std::unique_ptr<ChunkClient> newArea = std::make_unique<ChunkClient>(areaPosition, tileData);
        ChunkClient* area = newArea.get();
        areas[areaPosition] = std::move(newArea);

        area->generationComplete = false;

        area->Create();
        area->generationComplete = true;
        area->BakeSprite();
    }

    void WorldClient::OnPositionDataReceived(const std::vector<uint8_t>& data)
    {
        size_t offset = 1;

        uint32_t clientId = UnpackUint32(data, offset);
        uint64_t positionX = UnpackUint64(data, offset);
        uint64_t positionY = UnpackUint64(data, offset);

        if (!players.contains(clientId))
            return;

        PlayerClient* player = players[clientId].get();
        player->position.x = positionX;
        player->position.y = positionY;
    }

    void WorldClient::OnFactionDataReceived(const std::vector<uint8_t>& data)
    {
        size_t offset = 1;
        uint32_t factionId = UnpackUint32(data, offset);
        uint32_t ownerId = UnpackUint32(data, offset);
        uint32_t factionNameLength = UnpackUint32(data, offset);
        std::string factionName = UnpackString(data, offset, factionNameLength);

        AddFaction(factionId, ownerId, factionName);
    }

    void WorldClient::OnColonistPositionDataReceived(const std::vector<uint8_t>& data)
    {
        size_t offset = 1;

        uint32_t clientId = UnpackUint32(data, offset);
        uint32_t colonistId = UnpackUint32(data, offset);
        float colonistX = UnpackFloat(data, offset);
        float colonistY = UnpackFloat(data, offset);

        if (!players.contains(clientId))
            return;

        PlayerClient* player = players[clientId].get();
        if (!player->selectedColonists.contains(colonistId))
            return;

        ColonistClient* colonist = player->selectedColonists[colonistId];
        colonist->position = {colonistX, colonistY};
    }

    void WorldClient::OnConnectionAccepted(const std::vector<uint8_t>& data)
    {
        uint32_t id = client->clientId;
        AddPlayer(id, {0, 0});

        std::vector<uint8_t> request;
        request.push_back(FACTION_DATA_REQUEST);
        client->netInterface->SendToServer(request);
    }

    void WorldClient::OnClientConnected(const std::vector<uint8_t>& data)
    {
        size_t offset = 1;

        uint32_t peerId = UnpackUint32(data, offset);
        AddPlayer(peerId, {0, 0});
    }

    void WorldClient::OnClientDisconnected(const std::vector<uint8_t>& data)
    {
        size_t offset = 1;

        uint32_t peerId = UnpackUint32(data, offset);
        std::string peerUsername = client->GetUsername(peerId);

        RemovePlayer(peerId);
        RemoveFaction(factionOwnerToId[peerId]);
    }
}
