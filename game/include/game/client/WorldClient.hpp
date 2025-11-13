#pragma once

#include <vector>

#include "game/client/FactionClient.hpp"
#include "dapper2d/Camera.hpp"

#include "game/client/AreaClient.hpp"

namespace Game
{
    class Client;

    class WorldClient
    {
    private:
        Client* client = nullptr;

    public:
        std::unique_ptr<Engine::Camera> camera = nullptr;

        static inline std::string name{};

        std::map<Engine::Vec2<uint16_t>, std::unique_ptr<AreaClient>> areas{};
        std::map<Engine::Vec2<uint16_t>, bool> requestedAreas{};

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
        float minZoomForRendering = 0.5;

    public:
        explicit WorldClient(Client* _client);
        void Init();
        void Update(float delta);
        void Render();
        void HandleEvents(SDL_Event& event);
        void Clean();

        uint32_t AddFaction(uint32_t ownerId, const std::string& factionName);
        bool RemoveFaction(uint32_t factionId);

    private:
        void RequestWorldData();

        void OnWorldDataReceived(const std::vector<uint8_t>& data);
        void OnAreaDataReceived(const std::vector<uint8_t>& data);

        void OnConnectionAccepted(const std::vector<uint8_t>& data);
        void OnClientConnected(const std::vector<uint8_t>& data);
        void OnClientDisconnected(const std::vector<uint8_t>& data);
    };
}
