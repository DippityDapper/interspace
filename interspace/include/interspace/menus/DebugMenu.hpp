#pragma once

#include "igneous/scenes/Scene.hpp"

namespace Interspace
{
    class DebugMenu : Engine::Scene
    {
    public:
        void Init() override;
        void UI(Engine::InputLayer& layer) override;
    };
}
