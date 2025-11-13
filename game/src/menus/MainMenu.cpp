#include "game/menus/MainMenu.hpp"

#include "imgui.h"

#include "dapper2d/Scenes.hpp"
#include "dapper2d/Window.hpp"
#include "dapper2d/Engine.hpp"

#include "game/menus/JoinMenu.hpp"
#include "game/menus/MultiplayerMenu.hpp"
#include "game/menus/WorldCreationMenu.hpp"
#include "game/menus/WorldsMenu.hpp"
#include "SDL3/SDL_log.h"

namespace Game
{
    void MainMenu::Init()
    {
        CreateSubmenuScenes();
    }

    void MainMenu::Render()
    {
        ImGui::SetNextWindowSize(ImVec2(128, 256));
        ImGui::SetNextWindowPos({Engine::Window::viewport.x/2.0f,Engine::Window::viewport.y/2.0f}, 0, {0.5f,0.5f});

        ImGui::Begin
        ("Main Menu", nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove
        );

        const float singleplayerButtonWidth = 96.0f;
        const float singleplayerButtonHeight = 32.0f;
        ImVec2 singleplayerButtonSize(singleplayerButtonWidth, singleplayerButtonHeight);

        float availableWidth = ImGui::GetContentRegionAvail().x / 2.0f;
        float cursorPosX = ImGui::GetCursorPosX() + availableWidth - (singleplayerButtonWidth / 2.0f);
        ImGui::SetCursorPosX(cursorPosX);

        if (ImGui::Button("Singleplayer", singleplayerButtonSize))
        {
            Engine::Scenes::LoadScene("worlds_menu_singleplayer");
        }

        availableWidth = ImGui::GetContentRegionAvail().x / 2.0f;
        cursorPosX = ImGui::GetCursorPosX() + availableWidth - (singleplayerButtonWidth / 2.0f);
        ImGui::SetCursorPosX(cursorPosX);

        if (ImGui::Button("Multiplayer", singleplayerButtonSize))
        {
            Engine::Scenes::LoadScene("multiplayer_menu");
        }

        const float exitButtonWidth = 64.0f;
        const float exitButtonHeight = 32.0f;
        ImVec2 exitButtonSize(exitButtonWidth, exitButtonHeight);

        availableWidth = ImGui::GetContentRegionAvail().x / 2.0f;
        cursorPosX = ImGui::GetCursorPosX() + availableWidth - (exitButtonWidth / 2.0f);
        ImGui::SetCursorPosX(cursorPosX);

        float availableHeight = ImGui::GetContentRegionAvail().y;
        float cursorPosY = ImGui::GetCursorPosY() + availableHeight - exitButtonHeight;
        ImGui::SetCursorPosY(cursorPosY);

        if (ImGui::Button("Exit", exitButtonSize))
        {
            Engine::Engine::Quit();
        }

        ImGui::End();
    }

    void MainMenu::CreateSubmenuScenes()
    {
        if (!Engine::Scenes::SceneExists("worlds_menu_singleplayer"))
        {
            WorldsMenu* worldMenuSinglePlayerScene = new WorldsMenu();
            worldMenuSinglePlayerScene->isHostingMenu = false;
            worldMenuSinglePlayerScene->prevMenu = "main_menu";
            Engine::Scenes::CreateScene(worldMenuSinglePlayerScene, "worlds_menu_singleplayer");
        }
        if (!Engine::Scenes::SceneExists("multiplayer_menu"))
        {
            Engine::Scenes::CreateScene(new MultiplayerMenu(), "multiplayer_menu");
        }
        if (!Engine::Scenes::SceneExists("world_creation_menu"))
        {
            Engine::Scenes::CreateScene(new WorldCreationMenu(), "world_creation_menu");
        }
        if (!Engine::Scenes::SceneExists("worlds_menu_multiplayer"))
        {
            WorldsMenu* worldMenuMultiplayerScene = new WorldsMenu();
            worldMenuMultiplayerScene->isHostingMenu = true;
            worldMenuMultiplayerScene->prevMenu = "multiplayer_menu";
            Engine::Scenes::CreateScene(worldMenuMultiplayerScene, "worlds_menu_multiplayer");
        }
        if (!Engine::Scenes::SceneExists("join_menu"))
        {
            Engine::Scenes::CreateScene(new JoinMenu(), "join_menu");
        }
    }
}
