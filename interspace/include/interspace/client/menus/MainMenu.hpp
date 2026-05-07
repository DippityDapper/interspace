#pragma once

#include "interspace/client/menus/JoinMenu.hpp"
#include "interspace/client/menus/MultiplayerMenu.hpp"
#include "interspace/client/menus/UniverseCreationMenu.hpp"
#include "interspace/client/menus/UniversesMenu.hpp"
#include "igneous/scenes/Scene.hpp"

namespace Interspace
{
    class MainMenu : public Engine::Scene
    {
      public:
        UniversesMenu* singleplayerUniverseMenu = nullptr;
        UniversesMenu* multiplayerUniverseMenu = nullptr;
        MultiplayerMenu* multiplayerMenu = nullptr;
        UniverseCreationMenu* universeCreationMenu = nullptr;
        JoinMenu* joinMenu = nullptr;

      private:
        void OnCreated() override;
        void HandleInputs(Engine::InputLayer& layer) override;

        void CreateSubmenuScenes();
    };
}