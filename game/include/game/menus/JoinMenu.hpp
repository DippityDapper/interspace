#pragma once

#include "dapper2d/Scene.hpp"

namespace Game
{
    class JoinMenu : public Engine::Scene
    {
    private:
        char ipLineEdit[32] = "127.0.0.1";
        // char portLineEdit[5] = "";
        char usernameLineEdit[32] = "rocky";

    private:
        void Init() override;
        // void Update(float delta) override;
        void Render() override;
        // void HandleEvents(SDL_Event& event) override;
        // void Clean() override;

        bool JoinWorld();
    };
}
