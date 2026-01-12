#pragma once

#include "igneous/scenes/Scene.hpp"

namespace Interspace
{
    class MultiplayerMenu : public Engine::Scene
    {
    public:
        void Init() override;
        void Render() override;
    };
}
