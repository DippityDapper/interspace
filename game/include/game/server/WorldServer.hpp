#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>

#include "enet/enet.h"
#include "SDL3/SDL_events.h"

#include "dapper2d/Vec2.hpp"

#include "game/server/FactionServer.hpp"
#include "game/server/PlayerServer.hpp"
#include "game/server/ChunkServer.hpp"

namespace Game
{
    class Server;

    class WorldServer
    {
    private:
        Server* server = nullptr;

    public:
        static inline std::string name{};

        std::map<Engine::Vec2<uint16_t>, std::unique_ptr<ChunkServer>> areas{};

        std::map<uint32_t, std::unique_ptr<PlayerServer>> players{};

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

        bool AddPlayer(uint32_t clientId, const Engine::Vec2<uint64_t>& position);
        bool RemovePlayer(uint32_t clientId);

        uint32_t AddFaction(uint32_t ownerId, const std::string& factionName);
        bool RemoveFaction(uint32_t factionId);
        FactionServer* GetFaction(uint32_t factionId);

    private:
        void ConnectNetEvents();

        void OnWorldDataRequest(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnAreaDataRequest(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnFactionDataRequest(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnColonistPositionDataRequest(const std::vector<uint8_t>& data, ENetPeer* from);

        void OnPositionPacketReceived(const std::vector<uint8_t>& data, _ENetPeer* from);

        void OnClientConnected(const std::vector<uint8_t>& data, ENetPeer* from);
        void OnClientDisconnected(const std::vector<uint8_t>& data, ENetPeer* from);
    };
}
