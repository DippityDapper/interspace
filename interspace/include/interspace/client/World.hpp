#pragma once

#include <map>

#include "SDL3/SDL_events.h"

#include "interspace/client/Client.hpp"
#include "interspace/world/WorldData.hpp"
#include "interspace/client/Chunk.hpp"
#include "interspace/client/Faction.hpp"
#include "interspace/game/Camera.hpp"

namespace Interspace::Client
{
    class World
    {
    private:
        Client* client = nullptr;
        bool disconnectRequested = false;

        std::string worldName{};
        std::unique_ptr<Camera> camera = nullptr;

    public:
        static inline std::unique_ptr<WorldData> worldData{};

        std::map<uint16_t, std::unique_ptr<Chunk>> chunks{};
        std::map<uint8_t, std::unique_ptr<Faction>> factions{};

    public:
        explicit World(Client* _client);

        void Init();
        void Update(float delta);
        void Render();
        void HandleEvents(SDL_Event& event);
        void Clean();

    private:
        void RegisterNetEvents();

        void OnDisconnectAcknowledged(const std::vector<uint8_t>& data);

        void OnWorldDataReceived(const std::vector<uint8_t>& data);
        void OnFactionDataReceived(const std::vector<uint8_t>& data);
        void OnColonistPositionDataReceived(const std::vector<uint8_t>& data);
        void OnColonistSelectedDataReceived(const std::vector<uint8_t>& data);
        void OnColonistDeselectedAllDataReceived(const std::vector<uint8_t>& data);
        void OnColonistDeselectedDataReceived(const std::vector<uint8_t>& data);
    };
}
