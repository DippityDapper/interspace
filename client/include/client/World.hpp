#pragma once

#include <map>
#include <memory>

#include "dapper2d/Scene.hpp"
#include "dapper2d/Vec2.hpp"

#include "client/ClientEntity.hpp"

namespace Game
{
    class Area;
    class Camera;

    class World : public Engine::Scene
    {
    public:
        Camera* camera = nullptr;

        std::map<Engine::Vec2<int>, Area*> areas{};

        static uint32_t worldSeed;
        static int WORLD_SIZE_X;
        static int WORLD_SIZE_Y;
        float minZoomForRendering = 0.5;

        std::map<int, std::unique_ptr<ClientEntity>> entities{};

    private:
        void SaveWorld();

    public:
        void Init() override;
        void HandleEvents(SDL_Event& event) override;
        void Update(float delta) override;
        void Render() override;
        void RenderAreas();
        void RenderEntities();
        void Clean() override;
    };
}
