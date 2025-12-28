#pragma once

#include <map>

#include "SDL3/SDL_events.h"

#include "game/client/Client.hpp"
#include "game/world/WorldData.hpp"
#include "game/client/Chunk.hpp"
#include "game/client/Faction.hpp"

namespace Game::Client
{
    class World
    {
    private:
        Client* client = nullptr;
        std::string worldName{};

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

        void OnWorldDataReceived(const std::vector<uint8_t>& data);
        void OnFactionDataReceived(const std::vector<uint8_t>& data);
    };
}
