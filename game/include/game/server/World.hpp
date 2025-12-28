#pragma once

#include "SDL3/SDL_events.h"

#include "game/server/Server.hpp"
#include "game/world/WorldData.hpp"
#include "game/server/Chunk.hpp"
#include "game/server/Faction.hpp"

namespace Game::Server
{
    class World
    {
    private:
        Server* server = nullptr;

        uint32_t seed = 0;
        std::string worldName{};

    public:
        static inline std::unique_ptr<WorldData> worldData{};

        std::map<uint16_t, std::unique_ptr<Chunk>> chunks{};
        std::map<uint16_t, std::unique_ptr<Faction>> factions{};

        uint16_t nextFactionId = 1;
        uint16_t nextEntityId = 1;

    public:
        World(Server* _server, const std::string& _worldName);

        void Init();
        void Update(float delta);
        void Render();
        void HandleEvents(SDL_Event& event);
        void Clean();

        uint16_t AddFaction(uint32_t ownerId);

    private:
        void RegisterNetEvents();

        void OnClientConnected(const std::vector<uint8_t>& data, ENetPeer* from);

        void SendWorldData(ENetPeer* to);
        void SendFactionData(ENetPeer* to);
        void BroadcastFactionData(uint16_t factionId);
    };
}
