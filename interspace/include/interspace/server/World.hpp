#pragma once

#include <queue>

#include "igneous/scenes/Scene.hpp"
#include "interspace/server/Server.hpp"
#include "interspace/world/WorldData.hpp"
#include "interspace/server/Chunk.hpp"
#include "interspace/server/Faction.hpp"

namespace Interspace::Server
{
    class World : public Engine::Scene
    {
      private:
        Server* server = nullptr;

        uint32_t seed = 0;
        std::string worldName{};

        float serverClock = 0.05f;
        float serverTimer = 0.0f;

        float autosaveClock = 5.0f;
        float autosaveTimer = 0.0f;

        float chunkGenerationClock = 1.0f;
        float chunkGenerationTimer = 0.0f;

      public:
        static inline std::unique_ptr<WorldData> worldData{};

        std::map<Engine::Vec2<uint16_t>, std::unique_ptr<Chunk>> chunks{};
        std::queue<Chunk*> chunkQueue{};
        std::unordered_map<uint16_t, std::unique_ptr<Faction>> factions{};

        uint16_t nextFactionId = 1;
        uint16_t nextEntityId = 1;

      public:
        void Init() override;
        void Update(float delta) override;
        void Render() override;
        void Clean() override;

      public:
        void InitWorld(Server* _server, const std::string& _worldName);
        void InitFactions();

        uint16_t AddFaction(const std::string& factionName, uint32_t ownerId);
        uint16_t AddColonistToFaction(uint16_t factionId, const std::string& colonistName);

        bool DeleteFaction(uint16_t factionId);

        bool JoinFaction(uint16_t factionId, uint32_t playerId);
        bool JoinFaction(uint32_t playerId);

        bool LeaveFaction(uint16_t factionId, uint32_t playerId);

        void BeginChunkGeneration();
        void GenerateChunks();

      private:
        void AutoSave();

      private:
        void RegisterNetEvents();

        void OnClientConnected(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnClientDisconnected(const std::vector<uint8_t>& data, ENetPeer* from);

        void OnColonistPositionRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnColonistSelectRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnColonistDeselectAllRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnColonistDeselectRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnCreateColonistRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from);

        void BroadcastColonistPositionData();
        void BroadcastColonistDeselectAllData(uint32_t clientId);

        void SendWorldData(ENetPeer* to);
        void SendChunkData(ENetPeer* to, uint32_t playerId);
        void BroadcastChunkData(Chunk* chunk);

        void OnCreateFactionRequestReceived(const std::vector<uint8_t>& data, ENetPeer* from);
        void SendFactionData(ENetPeer* to);
        void BroadcastFactionData(uint16_t factionId);
    };
}
