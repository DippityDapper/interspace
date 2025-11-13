#pragma once

#include <queue>

#include "dapper2d/Scene.hpp"

namespace Game
{
    class MainMenu: public Engine::Scene
    {
    private:
        void Init() override;
        // void Update(float delta) override;
        void Render() override;
        // void HandleEvents(SDL_Event& event) override;
        // void Clean() override;

        void CreateSubmenuScenes();
    };
}