#pragma once

#include <map>
#include <memory>

#include "dapper2d/Scene.hpp"
#include "dapper2d/Vec2.hpp"

#include "client/ClientEntity.hpp"

namespace Engine
{
    class Camera;
}

namespace Game
{
    class Area;

    class World : public Engine::Scene
    {
    public:
        Engine::Camera* camera = nullptr;

        Engine::Vec2<int> currentAreaPosition{0, 0};
        std::map<Engine::Vec2<int>, Area*> areas{};
        Engine::Vec2<int> worldSize{0, 0};

        std::map<int, std::unique_ptr<ClientEntity>> entities{};

    public:
        void Init() override;
        void HandleEvents(SDL_Event& event) override;
        Area* GetCurrentArea();
        void Update(float delta) override;
        void Render() override;
        void RenderEntities();
        void Clean() override;
    };
}
