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

        Engine::Vec2<int> cameraAreaPosition{0, 0};
        std::map<Engine::Vec2<int>, Area*> areas{};

        static const int WORLD_SIZE_X;
        static const int WORLD_SIZE_Y;

        std::map<int, std::unique_ptr<ClientEntity>> entities{};

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
