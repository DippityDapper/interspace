#pragma once

#include "interspace/client/WorldGenerator.hpp"

#include <map>
#include <queue>

#include "igneous/scenes/Scene.hpp"

#include "interspace/client/Client.hpp"
#include "interspace/world/WorldData.hpp"
#include "interspace/client/Chunk.hpp"
#include "interspace/client/Faction.hpp"
#include "interspace/game/Camera.hpp"

namespace Interspace::Client
{
    class World : public Engine::Scene
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
        std::unique_ptr<WorldGenerator> worldGenerator = nullptr;
        static inline std::unique_ptr<WorldData> worldData{};

        std::map<Engine::Vec2<uint16_t>, std::unique_ptr<Chunk>> chunks{};
        std::queue<std::vector<uint8_t>> chunkDataQueue{};
        std::map<uint8_t, std::unique_ptr<Faction>> factions{};

      public:
        void Init() override;
        void Update(float delta) override;
        void UI() override;
        void HandleEvents(Engine::InputLayer& layer) override;
        void Clean() override;

        void SendPosition();

      public:
        void InitWorld(Client* _client);

      private:
        void RequestDisconnect();
        void RequestCreateColonist(uint16_t factionId, const std::string& colonistName);
        void RequestMoveColonist(uint16_t factionId, uint32_t colonistId, float x, float y);
        void RequestColonistSelect(uint16_t factionId, uint32_t colonistId);
        void RequestColonistDeselect(uint16_t factionId, uint32_t colonistId);
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
