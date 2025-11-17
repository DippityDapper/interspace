#include "game/server/WorldServer.hpp"

#include "game/network/NetworkPackets.hpp"
#include "game/server/Server.hpp"

namespace Game
{
    void WorldServer::ConnectNetEvents()
    {
        server->ConnectToEvent(WORLD_DATA_REQUEST, this, &WorldServer::OnWorldDataRequest);
        server->ConnectToEvent(AREA_DATA_REQUEST, this, &WorldServer::OnAreaDataRequest);
        server->ConnectToEvent(FACTION_DATA_REQUEST, this, &WorldServer::OnFactionDataRequest);

        server->ConnectToEvent(CLIENT_CONNECTED, this, &WorldServer::OnClientConnected);
        server->ConnectToEvent(CLIENT_DISCONNECTED, this, &WorldServer::OnClientDisconnected);

        server->ConnectToEvent(POSITION_PACKET, this, &WorldServer::OnPositionPacketReceived);

        server->ConnectToEvent(COLONIST_POSITION_REQUEST, this, &WorldServer::OnColonistPositionDataRequest);
    }

    void WorldServer::OnWorldDataRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        std::vector<uint8_t> worldData;
        worldData.push_back(WORLD_DATA_PACKET);

        uint32_t worldNameLen = static_cast<uint32_t>(name.size());
        PackBytes(worldData, &worldNameLen, sizeof(uint32_t));
        PackBytes(worldData, name.data(), worldNameLen);
        PackBytes(worldData, &worldSizeX, sizeof(uint16_t));
        PackBytes(worldData, &worldSizeY, sizeof(uint16_t));

        server->netInterface->SendToClient(from, worldData);
    }

    void WorldServer::OnAreaDataRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        size_t offset = 1;

        uint16_t requestedAreaX = UnpackUint16(data, offset);
        uint16_t requestedAreaY = UnpackUint16(data, offset);
        Engine::Vec2<uint16_t> requestedAreaPosition(requestedAreaX, requestedAreaY);

        ChunkServer* area = new ChunkServer(requestedAreaPosition, seed);
        if (!areas.contains(requestedAreaPosition))
        {
            area->Create();
            area->generationComplete = true;
        }
        else
        {
            area = areas[requestedAreaPosition].get();
        }

        std::vector<uint8_t> areaData = area->Serialize();

        std::vector<uint8_t> response;
        response.push_back(AREA_DATA_PACKET);
        response.insert(response.end(), areaData.begin(), areaData.end());
        server->netInterface->SendToClient(from, response);

        delete area;
    }

    void WorldServer::OnFactionDataRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        for (const auto& kvp : factions)
        {
            std::vector<uint8_t> factionData;
            factionData.push_back(FACTION_DATA_PACKET);

            FactionServer* faction = kvp.second.get();
            PackBytes(factionData, &faction->id, sizeof(uint32_t));
            PackBytes(factionData, &faction->ownerId, sizeof(uint32_t));

            uint32_t factionNameLength = static_cast<uint32_t>(faction->name.size());
            PackBytes(factionData, &factionNameLength, sizeof(uint32_t));
            PackBytes(factionData, faction->name.data(), factionNameLength);

            server->netInterface->SendToClient(from, factionData);
        }
    }

    void WorldServer::OnColonistPositionDataRequest(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        size_t offset = 1;

        uint32_t clientId = UnpackUint32(data, offset);
        uint32_t colonistId = UnpackUint32(data, offset);
        float mouseGlobalX = UnpackFloat(data, offset);
        float mouseGlobalY = UnpackFloat(data, offset);

        if (!players.contains(clientId))
            return;

        PlayerServer* player = players[clientId].get();
        if (!player->selectedColonists.contains(colonistId))
            return;

        ColonistServer* colonist = player->selectedColonists[colonistId];
        colonist->position = {mouseGlobalX, mouseGlobalY};

        std::vector<uint8_t> colonistData;
        colonistData.push_back(COLONIST_POSITION_PACKET);

        PackBytes(colonistData, &clientId, sizeof(uint32_t));
        PackBytes(colonistData, &colonistId, sizeof(uint32_t));
        PackBytes(colonistData, &colonist->position.x, sizeof(float));
        PackBytes(colonistData, &colonist->position.y, sizeof(float));

        for (const auto& kvp : server->GetPeers())
            server->netInterface->SendToClient(kvp.second, colonistData);
    }


    void WorldServer::OnPositionPacketReceived(const std::vector<uint8_t>& data, _ENetPeer* from)
    {
        size_t offset = 1;

        uint32_t clientId = UnpackUint32(data, offset);
        uint64_t positionX = UnpackUint64(data, offset);
        uint64_t positionY = UnpackUint64(data, offset);

        if (!players.contains(clientId))
            return;

        PlayerServer* player = players[clientId].get();
        player->position.x = positionX;
        player->position.y = positionY;

        for (const auto& kvp : players)
        {
            if (kvp.first == clientId)
                continue;
            server->netInterface->SendToClient(server->GetPeer(kvp.first), data);
        }
    }

    void WorldServer::OnClientConnected(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        size_t offset = 1;

        uint32_t peerId = UnpackUint32(data, offset);
        uint32_t usernameLength = UnpackUint32(data, offset);
        std::string peerUsername = UnpackString(data, offset, usernameLength);

        AddPlayer(peerId, {0, 0});
        uint32_t factionId = AddFaction(peerId, peerUsername);

        if (factionId == 0)
            return;

        FactionServer* faction = GetFaction(factionId);
        std::vector<uint8_t> factionData;
        factionData.push_back(FACTION_DATA_PACKET);

        PackBytes(factionData, &faction->id, sizeof(uint32_t));
        PackBytes(factionData, &faction->ownerId, sizeof(uint32_t));

        uint32_t factionNameLength = static_cast<uint32_t>(faction->name.size());
        PackBytes(factionData, &factionNameLength, sizeof(uint32_t));
        PackBytes(factionData, faction->name.data(), factionNameLength);

        for (const auto& peer : server->GetPeers())
        {
            if (peer.second == from)
                continue;
            server->netInterface->SendToClient(peer.second, factionData);
        }
    }

    void WorldServer::OnClientDisconnected(const std::vector<uint8_t>& data, ENetPeer* from)
    {
        size_t offset = 1;

        uint32_t peerId = UnpackUint32(data, offset);
        std::string peerUsername = server->GetUsername(peerId);

        RemovePlayer(peerId);
        RemoveFaction(factionOwnerToId[peerId]);
    }
}
