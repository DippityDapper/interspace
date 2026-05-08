#include "interspace/client/menus/MainMenu.hpp"

#include "imgui.h"
#include "igneous/engine/CFGParser.hpp"

#include "igneous/scenes/SceneManager.hpp"
#include "igneous/rendering/Window.hpp"
#include "igneous/engine/Engine.hpp"
#include "interspace/client/sounds/SoundManager.hpp"

#include "interspace/client/menus/JoinMenu.hpp"
#include "interspace/client/menus/MultiplayerMenu.hpp"
#include "interspace/client/menus/UniverseCreationMenu.hpp"
#include "interspace/client/menus/UniversesMenu.hpp"

namespace Interspace
{
    void MainMenu::OnCreated()
    {
        CreateSubmenuScenes();
    }

    void MainMenu::HandleInputs(Engine::InputLayer& layer)
    {
        if (!layer.Is("ui"))
            return;

        float windowWidth = 192.0f;
        float windowHeight = 256.0f;

        ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
        ImGui::SetNextWindowPos(
                {Engine::Window::viewport.x / 2.0f, Engine::Window::viewport.y / 2.0f},
                0,
                {0.5f, 0.5f});

        ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

        float buttonWidth = 96.0f;
        float buttonHeight = 32.0f;
        float exitButtonWidth = 64.0f;

        float availableWidth = ImGui::GetContentRegionAvail().x;

        ImVec2 buttonSize(buttonWidth, buttonHeight);
        ImGui::SetCursorPosX((availableWidth - buttonWidth) * 0.5f);

        if (ImGui::Button("Singleplayer", buttonSize))
        {
            SoundManager::PlaySound("button_1", 1.0f);
            root->LoadScene("universes_menu_singleplayer");
        }

        ImGui::SetCursorPosX((availableWidth - buttonWidth) * 0.5f);

        if (ImGui::Button("Multiplayer", buttonSize))
        {
            SoundManager::PlaySound("button_1", 1.0f);
            root->LoadScene("multiplayer_menu");
        }

        ImVec2 exitSize(exitButtonWidth, buttonHeight);

        float availableHeight = ImGui::GetContentRegionAvail().y;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + availableHeight - buttonHeight);
        ImGui::SetCursorPosX((availableWidth - exitButtonWidth) * 0.5f);

        if (ImGui::Button("Exit", exitSize))
        {
            Engine::Engine::Quit();
        }

        ImGui::End();
    }

    void MainMenu::CreateSubmenuScenes()
    {
        if (!singleplayerUniverseMenu)
        {
            singleplayerUniverseMenu = root->AddScene<UniversesMenu>("universes_menu_singleplayer", "main_menus", false);
            singleplayerUniverseMenu->isHostingMenu = false;
            singleplayerUniverseMenu->prevMenu = "main_menu";
        }
        if (!multiplayerMenu)
        {
            multiplayerMenu = root->AddScene<MultiplayerMenu>("multiplayer_menu", "main_menus", false);
        }
        if (!universeCreationMenu)
        {
            universeCreationMenu = root->AddScene<UniverseCreationMenu>("universe_creation_menu", "main_menus", false);
        }
        if (!multiplayerUniverseMenu)
        {
            multiplayerUniverseMenu = root->AddScene<UniversesMenu>("universes_menu_multiplayer", "main_menus", false);
            multiplayerUniverseMenu->isHostingMenu = true;
            multiplayerUniverseMenu->prevMenu = "multiplayer_menu";
        }
        if (!joinMenu)
        {
            joinMenu = root->AddScene<JoinMenu>("join_menu", "main_menus", false);
        }
    }
}