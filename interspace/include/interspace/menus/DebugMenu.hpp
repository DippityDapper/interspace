#pragma once

#include "igneous/Scene.hpp"

namespace Interspace
{
    class DebugMenu : Engine::Scene
    {
    public:
        void Init() override;
        void Render() override;
    };
}
