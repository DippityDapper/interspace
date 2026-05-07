#pragma once

#include "igneous/scenes/Scene.hpp"

namespace Interspace
{
    class MultiplayerMenu : public Engine::Scene
    {
      public:
        void HandleInputs(Engine::InputLayer& layer) override;
    };
}
