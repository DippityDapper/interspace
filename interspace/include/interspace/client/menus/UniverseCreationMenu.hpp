#pragma once

#include <memory>

#include "igneous/scenes/Scene.hpp"

namespace Interspace
{
    class UniverseCreationMenu : public Engine::Scene
    {
      private:
        char universeNameLineEdit[16] = "";
        char universeSeedLineEdit[32] = "";
        std::string message;

      public:
        std::string prevMenu = "worlds_menu_singleplayer";

      private:
        bool CreateUniverse();

      private:
        void HandleInputs(Engine::InputLayer& layer) override;
    };
}
