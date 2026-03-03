#include "interspace/menus/MainMenu.hpp"

#include "imgui.h"
#include "igneous/engine/CFGParser.hpp"

#include "igneous/scenes/SceneManager.hpp"
#include "igneous/rendering/Window.hpp"
#include "igneous/engine/Engine.hpp"
#include "interspace/game/Sounds.hpp"

#include "interspace/menus/JoinMenu.hpp"
#include "interspace/menus/MultiplayerMenu.hpp"
#include "interspace/menus/WorldCreationMenu.hpp"
#include "interspace/menus/WorldsMenu.hpp"

namespace Interspace
{
    void MainMenu::Init()
    {
        CreateSubmenuScenes();
    }

    void MainMenu::OnActiveChanged(bool value)
    {
        if (!value)
            return;

        Engine::CFGParser::LoadConfig("game_config.cfg", "game_config");
        try
        {
            username = Engine::CFGParser::GetString("game_config", "username");
        }
        catch (const std::exception& e)
        {
            Engine::CFGParser::WriteString("game_config", "username", "");
            username = "";
        }

        strcpy(usernameLineEdit, username.c_str());
    }

    void MainMenu::UI()
    {
        float windowWidth = 192.0f;
        float windowHeight = 256.0f;

        ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
        ImGui::SetNextWindowPos(
                {Engine::Window::viewport.x / 2.0f, Engine::Window::viewport.y / 2.0f},
                0,
                {0.5f, 0.5f});

        ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

        float usernameEditWidth = 144.0f;
        float buttonWidth = 96.0f;
        float buttonHeight = 32.0f;
        float exitButtonWidth = 64.0f;

        float availableWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX((availableWidth - usernameEditWidth) * 0.5f);

        if (strcmp(usernameLineEdit, username.c_str()) != 0)
        {
            username = usernameLineEdit;
            Engine::CFGParser::LoadConfig("game_config.cfg", "game_config");
            Engine::CFGParser::WriteString("game_config", "username", username);
            Engine::CFGParser::SaveConfig("game_config.cfg", "game_config");
        }

        ImGui::PushItemWidth(usernameEditWidth);
        ImGui::InputText("##username", usernameLineEdit, sizeof(usernameLineEdit));
        ImGui::PopItemWidth();

        ImVec2 labelSize = ImGui::CalcTextSize("Username");
        ImGui::SetCursorPosX((availableWidth - labelSize.x) * 0.5f);
        ImGui::TextDisabled("Username");

        ImGui::Spacing();

        if (!errorMessage.empty())
        {
            ImVec2 errorSize = ImGui::CalcTextSize(errorMessage.c_str());
            ImGui::SetCursorPosX((availableWidth - errorSize.x) * 0.5f);
            ImGui::Text(errorMessage.c_str());
            ImGui::Spacing();
        }

        ImVec2 buttonSize(buttonWidth, buttonHeight);
        ImGui::SetCursorPosX((availableWidth - buttonWidth) * 0.5f);

        if (ImGui::Button("Singleplayer", buttonSize))
        {
            Sounds::PlaySound("button_1", 1.0f);

            if (!username.empty())
            {
                root->LoadScene("worlds_menu_singleplayer");
                errorMessage = "";
            }
            else
            {
                errorMessage = "Username is empty.";
            }
        }

        ImGui::SetCursorPosX((availableWidth - buttonWidth) * 0.5f);

        if (ImGui::Button("Multiplayer", buttonSize))
        {
            Sounds::PlaySound("button_1", 1.0f);

            if (!username.empty())
            {
                root->LoadScene("multiplayer_menu");
                errorMessage = "";
            }
            else
            {
                errorMessage = "Username is empty.";
            }
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
        if (!singleplayerWorldMenu)
        {
            singleplayerWorldMenu = root->AddScene<WorldsMenu>("worlds_menu_singleplayer", false);
            singleplayerWorldMenu->isHostingMenu = false;
            singleplayerWorldMenu->prevMenu = "main_menu";
        }
        if (!multiplayerMenu)
        {
            multiplayerMenu = root->AddScene<MultiplayerMenu>("multiplayer_menu", false);
        }
        if (!worldCreationMenu)
        {
            worldCreationMenu = root->AddScene<WorldCreationMenu>("world_creation_menu", false);
        }
        if (!multiplayerWorldMenu)
        {
            multiplayerWorldMenu = root->AddScene<WorldsMenu>("worlds_menu_multiplayer", false);
            multiplayerWorldMenu->isHostingMenu = true;
            multiplayerWorldMenu->prevMenu = "multiplayer_menu";
        }
        if (!joinMenu)
        {
            joinMenu = root->AddScene<JoinMenu>("join_menu", false);
        }
    }
}