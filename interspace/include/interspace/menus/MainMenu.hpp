#pragma once

#include "interspace/menus/JoinMenu.hpp"
#include "interspace/menus/MultiplayerMenu.hpp"
#include "interspace/menus/WorldCreationMenu.hpp"
#include "interspace/menus/WorldsMenu.hpp"
#include "igneous/scenes/Scene.hpp"

namespace Interspace
{
    class MainMenu : public Engine::Scene
    {
      private:
        char usernameLineEdit[16] = "";
        std::string username{};
        std::string errorMessage{};

      public:
        WorldsMenu* singleplayerWorldMenu = nullptr;
        WorldsMenu* multiplayerWorldMenu = nullptr;
        MultiplayerMenu* multiplayerMenu = nullptr;
        WorldCreationMenu* worldCreationMenu = nullptr;
        JoinMenu* joinMenu = nullptr;

      private:
        void Init() override;
        void OnActiveChanged(bool value) override;
        void Render() override;

        void CreateSubmenuScenes();
    };
}