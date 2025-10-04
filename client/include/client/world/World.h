#pragma once

#include <map>

#include "SDL3/SDL.h"

#include "common/packets/Packets.h"

#include "client/world/Area.h"
#include "client/camera/Camera.h"
#include "client/engine/Scene.h"
#include "client/engine/Renderer.h"

namespace Game
{
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
