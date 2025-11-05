#pragma once

#include <map>
#include <memory>

#include "dapper2d/Scene.hpp"
#include "dapper2d/Vec2.hpp"

#include "client/ClientEntity.hpp"
#include "client/Area.hpp"
#include "client/Camera.hpp"

namespace Game
{
    class World : public Engine::Scene
    {
    private:
        std::string name{""};

    public:
        std::unique_ptr<Camera> camera = nullptr;

        std::map<Engine::Vec2<int>, std::unique_ptr<Game::Area>> areas;

        static uint32_t worldSeed;
        static int WORLD_SIZE_X;
        static int WORLD_SIZE_Y;
        static int REGION_SIZE;
        float minZoomForRendering = 0.5;

        std::map<int, std::unique_ptr<ClientEntity>> entities{};

    private:
        void SaveWorld();
        bool FileExists(const std::string& fileName);
        void SaveAreaToRegion(Game::Area* area, int rx, int ry, const std::string& filePath);
        std::unique_ptr<Game::Area> LoadAreaFromRegion(int areaX, int areaY, int regionSize);

    public:
        World(const std::string& worldName);
        void Init() override;
        void HandleEvents(SDL_Event& event) override;
        void Update(float delta) override;
        void Render() override;
        void RenderAreas();
        void RenderEntities();
        void Clean() override;
    };
}
