#pragma once

#include <vector>

#include "dapper2d/Camera.hpp"

#include "game/world/AreaClient.hpp"

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

    private:
        void RequestWorldData();

        void OnWorldDataReceived(const std::vector<uint8_t>& data);
        void OnAreaDataReceived(const std::vector<uint8_t>& data);
    };
}
