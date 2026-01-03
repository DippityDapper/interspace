#pragma once

#include "igneous/Scene.hpp"

namespace Interspace
{
    class MultiplayerMenu : public Engine::Scene
    {
    public:
        void Init() override;
        void Render() override;
    };
}
