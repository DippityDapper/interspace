#pragma once

#include <vector>

#include "PlayerClient.hpp"
#include "game/client/FactionClient.hpp"

#include "game/client/ChunkClient.hpp"
#include "game/game/Camera.hpp"

namespace Game
{
    class Client;

    class WorldClient
    {
    private:
        Client* client = nullptr;

        const float sendClock = 0.05f;
        float sendTimer = 0.0f;

    public:
        std::unique_ptr<Camera> camera = nullptr;
        static inline std::string name{};
        Engine::Vec2<uint64_t> previousCameraPosition{0, 0};

        std::map<Engine::Vec2<uint16_t>, std::unique_ptr<ChunkClient>> areas{};
        std::map<Engine::Vec2<uint16_t>, bool> requestedAreas{};

        std::map<uint32_t, std::unique_ptr<PlayerClient>> players{};

        std::map<uint32_t, std::unique_ptr<FactionClient>> factions{};
        std::map<uint32_t, std::string> factionIdToName{};
        std::map<std::string, uint32_t> factionNameToId{};
        std::map<uint32_t, uint32_t> factionOwnerToId{};

        uint32_t nextFactionId = 1;

        static inline uint16_t worldSizeX = 0;
        static inline uint16_t worldSizeY = 0;
        static inline const uint8_t AREA_SIZE = 64;
        static inline const uint8_t TILE_SIZE = 32;
        static inline const uint8_t REGION_SIZE = 32;
        static inline int renderDistance = 1;


    public:
        explicit WorldClient(Client* _client);

        // SCENE
        void Init();
        void Update(float delta);
        void Render();
        void HandleEvents(SDL_Event& event);
        void Clean();

        // DATA
        bool AddPlayer(uint32_t clientId, const Engine::Vec2<uint64_t>& position);
        bool RemovePlayer(uint32_t clientId);

        uint32_t AddFaction(uint32_t ownerId, const std::string& factionName);
        uint32_t AddFaction(uint32_t factionId, uint32_t ownerId, const std::string& factionName);
        bool RemoveFaction(uint32_t factionId);

        // UPDATES
        void SendPlayerPosition();
        void UpdateAreas(float delta);

        // RENDERING
        void RenderVisibleAreas();
        void DisplayPlayerNames();
        void RenderFactions();

    private:
        void ConnectNetEvents();

        // NET RECEIVED
        void OnWorldDataReceived(const std::vector<uint8_t>& data);
        void OnAreaDataReceived(const std::vector<uint8_t>& data);
        void OnPositionDataReceived(const std::vector<uint8_t>& data);
        void OnFactionDataReceived(const std::vector<uint8_t>& data);
        void OnColonistPositionDataReceived(const std::vector<uint8_t>& data);

        // NET OTHER
        void OnConnectionAccepted(const std::vector<uint8_t>& data);
        void OnClientConnected(const std::vector<uint8_t>& data);
        void OnClientDisconnected(const std::vector<uint8_t>& data);
    };
}
