#pragma once
#include "igneous/scenes/Scene.hpp"

namespace Interspace
{
    class PreambleMenu : public Engine::Scene
    {
      private:
        char usernameLineEdit[24] = "";
        std::string errorMessage{};

      private:
        void HandleInputs(Engine::InputLayer& layer) override;
    };
}
