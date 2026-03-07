#pragma once

#include "Player.hpp"
#include "ServerTiles.hpp"
#include "SQLiteCpp/Database.h"

#include "interspace/server/ServerWorldGenerator.hpp"
#include "igneous/scenes/Scene.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/world/WorldData.hpp"
#include "interspace/server/ServerFaction.hpp"

namespace Interspace::Server
{
    class ServerWorld : public Engine::Scene
    {
      private:
        Server* server = nullptr;

        float serverClock = 0.05f;
        float serverTimer = 0.0f;

        float autosaveClock = 5.0f;
        float autosaveTimer = 0.0f;

        float chunkGenerationClock = 0.05f;
        float chunkGenerationTimer = 0.0f;

      public:
        std::string worldName{};

        std::unique_ptr<WorldData> worldData{};
        std::unique_ptr<ServerWorldGenerator> worldGenerator = nullptr;
        std::unique_ptr<ServerTiles> tileRegistry = nullptr;

        std::unordered_map<client_id_t, std::unique_ptr<Player>> players{};
        std::unordered_map<faction_id_t, std::unique_ptr<ServerFaction>> factions{};

        faction_id_t nextFactionId = 1;
        entity_id_t nextEntityId = 1;

      public:
        void Init() override;
        void Update(float delta) override;
        void Clean() override;

      public:
        void InitWorld(Server* _server, const std::string& _worldName);
        void InitFactions();

        faction_id_t AddFaction(const std::string& factionName, client_id_t ownerId);
        entity_id_t AddColonistToFaction(faction_id_t factionId, const std::string& colonistName);

        bool DeleteFaction(faction_id_t factionId);

        bool JoinFaction(faction_id_t factionId, client_id_t clientId);
        bool JoinFaction(client_id_t clientId);

        bool LeaveFaction(faction_id_t factionId, client_id_t clientId);

      private:
        void AutoSave();
        void SendFactionCreateRequest(client_id_t clientId);
        void AcceptFactionRequest(ENetPeer* peer);
        void DenyFactionRequest(ENetPeer* peer);
        void BroadcastColonistSelection(faction_id_t factionId, entity_id_t colonistId, client_id_t clientId);
        void BroadcastColonistDeselection(faction_id_t factionId, entity_id_t colonistId, client_id_t clientId);

        void RegisterNetEvents();

        void OnClientConnected(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnClientDisconnected(const std::vector<uint8_t>& data, ENetPeer* from);

        void OnColonistPositionRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnColonistSelectRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnColonistDeselectAllRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnColonistDeselectRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnCreateColonistRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from);

        void BroadcastColonistPositionData();
        void BroadcastColonistDeselectAllPacket(client_id_t clientId);

        void SendWorldData(ENetPeer* to);
        void SendChunkData(ENetPeer* to, client_id_t clientId);

        void OnCreateFactionRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from);
        void SendFactionData(ENetPeer* to, client_id_t clientId);
        void BroadcastFactionData(faction_id_t factionId);

        void OnPlayerPositionReceived(const std::vector<uint8_t>& data, ENetPeer* from);

        void OnChunkRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from);
    };
}
