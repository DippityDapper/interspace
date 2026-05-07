#pragma once

#include <memory>

#include "igneous/engine/Camera.hpp"
#include "igneous/scenes/Scene.hpp"

namespace Interspace
{
    class UniverseCreationMenu : public Engine::Scene
    {
      private:
        std::unique_ptr<Engine::Camera> camera = nullptr;
        char universeNameLineEdit[16] = "";
        char universeSeedLineEdit[32] = "";
        std::string message;

      public:
        std::string prevMenu = "worlds_menu_singleplayer";

      private:
        bool CreateUniverse();

      private:
        void OnCreated() override;
        void HandleInputs(Engine::InputLayer& layer) override;
    };
}
