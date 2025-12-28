#pragma once

#include "igneous/Scene.hpp"

namespace Game
{
    class JoinMenu : public Engine::Scene
    {
    private:
        char ipLineEdit[32] = "127.0.0.1";
        char usernameLineEdit[32] = "rocky";

    private:
        void Init() override;
        void Render() override;

        bool JoinWorld();
    };
}
