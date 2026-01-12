#pragma once

#include "igneous/scenes/Scene.hpp"

namespace Interspace
{
    class JoinMenu : public Engine::Scene
    {
    private:
        char ipLineEdit[32] = "127.0.0.1";

    private:
        void Init() override;
        void Render() override;

        bool JoinWorld();
    };
}
