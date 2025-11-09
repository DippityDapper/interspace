#pragma once

#include "dapper2d/Scene.hpp"

namespace Game
{
    class Game: public Engine::Scene
    {
    private:
        void Init() override;
        void Update(float delta) override;
        void HandleEvents(SDL_Event& event) override;
        void Clean() override;
    };
}