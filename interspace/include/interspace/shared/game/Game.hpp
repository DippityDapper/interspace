#pragma once

#include "igneous/scenes/Scene.hpp"

namespace Interspace
{
    class Game : public Engine::Scene
    {
      private:
        void OnCreated() override;
    };
}
