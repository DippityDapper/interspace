#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>

#include "FactionServer.hpp"
#include "enet/enet.h"
#include "SDL3/SDL_events.h"

#include "dapper2d/Vec2.hpp"

#include "game/server/AreaServer.hpp"

namespace Game
{
    class Server;

    class WorldServer
    {
    private:
        Server* server = nullptr;

    public:
        static inline std::string name{};

        std::map<Engine::Vec2<uint16_t>, std::unique_ptr<AreaServer>> areas{};

        std::map<uint32_t, std::unique_ptr<FactionServer>> factions{};
        std::map<uint32_t, std::string> factionIdToName{};
        std::map<std::string, uint32_t> factionNameToId{};
        std::map<uint32_t, uint32_t> factionOwnerToId{};

        uint32_t nextFactionId = 1;

        static inline uint32_t seed = 0;
        static inline uint16_t worldSizeX = 0;
        static inline uint16_t worldSizeY = 0;
        static inline const uint8_t AREA_SIZE = 64;
        static inline const uint8_t TILE_SIZE = 32;
        static inline const uint8_t REGION_SIZE = 32;

    public:
        WorldServer(Server* _server, const std::string& worldName);
        void Init();
        void Update(float delta);
        void HandleEvents(SDL_Event& event);
        void Clean();

        uint32_t AddFaction(uint32_t ownerId, const std::string& factionName);
        bool RemoveFaction(uint32_t factionId);

    private:
        void OnWorldDataRequest(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnAreaDataRequest(const std::vector<uint8_t>& data, ENetPeer* from);

        void OnClientConnected(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnClientDisconnected(const std::vector<uint8_t>& data, ENetPeer* from);
    };
}
