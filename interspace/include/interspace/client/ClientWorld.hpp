#pragma once

#include "interspace/client/ClientTiles.hpp"
#include "interspace/client/ClientFaction.hpp"
#include "interspace/client/ClientWorldGenerator.hpp"
#include "igneous/scenes/Scene.hpp"
#include "interspace/game/Camera.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/world/WorldData.hpp"

#include <memory>
#include <queue>

namespace Interspace::Client
{
    class ClientWorld : public Engine::Scene
    {
      private:
        Client* client = nullptr;
        bool disconnectRequested = false;

        std::unique_ptr<Camera> camera = nullptr;
        Engine::Vec2<float> previousCameraPosition{};

        float chunkGenerationClock = 0.05f;
        float chunkGenerationTimer = 0.0f;

        float clientClock = 0.05f;
        float clientTimer = 0.0f;

      public:
        std::string worldName{};
        std::unique_ptr<ClientWorldGenerator> worldGenerator = nullptr;
        std::unique_ptr<WorldData> worldData{};
        std::unique_ptr<ClientTiles> tileRegistry = nullptr;
        std::map<faction_id_t, std::unique_ptr<ClientFaction>> factions{};

      public:
        void Init() override;
        void Update(float delta) override;
        void Render() override;
        void HandleEvents(Engine::InputLayer& layer) override;
        void Clean() override;

        void SendPosition();

      public:
        void InitWorld(Client* _client);

      private:
        void RequestDisconnect();
        void RequestCreateColonist(faction_id_t factionId, const std::string& colonistName);
        void RequestMoveColonist(faction_id_t factionId, entity_id_t colonistId, float x, float y);
        void RequestColonistSelect(faction_id_t factionId, entity_id_t colonistId);
        void RequestColonistDeselect(faction_id_t factionId, entity_id_t colonistId);
        void RequestColonistDeselectAll();

      private:
        void RegisterNetEvents();

        void OnDisconnectAcknowledged(const std::vector<uint8_t>& data);

        void OnWorldDataReceived(const std::vector<uint8_t>& data);
        void OnFactionDataReceived(const std::vector<uint8_t>& data);
        void OnCreateFactionRequestReceived(const std::vector<uint8_t>& data);
        void OnColonistPositionDataReceived(const std::vector<uint8_t>& data);
        void OnColonistSelectedPacketReceived(const std::vector<uint8_t>& data);
        void OnColonistDeselectedAllDataReceived(const std::vector<uint8_t>& data);
        void OnColonistDeselectedDataReceived(const std::vector<uint8_t>& data);

        void OnChunkPacketReceived(const std::vector<uint8_t>& data);
    };
}
