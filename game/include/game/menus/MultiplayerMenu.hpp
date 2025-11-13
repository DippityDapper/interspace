#pragma once

#include "dapper2d/Scene.hpp"

namespace Game
{
    class MultiplayerMenu : public Engine::Scene
    {
    public:
        void Init() override;
        // void Update(float delta) override;
        void Render() override;
        // void HandleEvents(SDL_Event& event) override;
        // void Clean() override;
    };
}
