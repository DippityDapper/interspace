#include "game/menus/WorldsMenu.hpp"

#include <fstream>
#include <filesystem>

#include "imgui.h"

#include "dapper2d/Scenes.hpp"
#include "dapper2d/Vec2.hpp"
#include "dapper2d/Window.hpp"
#include "game/game/Game.hpp"
#include "game/menus/MainMenu.hpp"

#include "game/menus/WorldCreationMenu.hpp"

namespace Game
{
    void WorldsMenu::Init()
    {
        worlds.clear();

        if (!std::filesystem::exists("worlds"))
        {
            std::filesystem::create_directory("worlds");
            return;
        }

        for (const auto& dir : std::filesystem::directory_iterator("worlds"))
        {
            if (!dir.exists() || !dir.is_directory())
                continue;

            std::string dirStr = dir.path().string();
            std::string worldName = dirStr.substr(dirStr.find('\\')+1, (dirStr.find('\\')+1 - dir.path().string().size()) - 1);
            worlds.push_back(worldName);
        }
    }

    void WorldsMenu::Render()
    {
        Engine::Vec2<int> viewport = Engine::Window::viewport;

        const float windowWidth = 384.0f;
        const float windowHeight = 256.0f;
        const float buttonWidth = 64.0f;
        const float buttonHeight = 24.0f;
        const float childRounding = 6.0f;
        const float containerPadding = 8.0f;
        const float entryPadding = 4.0f;
        const float deleteButtonOffsetMultiplier = 2.1f;

        const ImVec4 containerBgColor = ImVec4(0.15f, 0.24f, 0.66f, 0.2f);
        const ImVec4 entryBgColor = ImVec4(0.25f, 0.54f, 0.96f, 0.2f);

        ImVec2 buttonSize(buttonWidth, buttonHeight);

        ImGui::SetNextWindowSize({windowWidth, windowHeight});
        ImGui::SetNextWindowPos({viewport.x * 0.5f, viewport.y * 0.5f}, 0, {0.5f, 0.5f});

        ImGui::Begin(
            "Worlds", nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_AlwaysAutoResize
        );

        float footerHeight = buttonHeight + ImGui::GetStyle().ItemSpacing.y * 2;
        float childHeight = ImGui::GetContentRegionAvail().y - footerHeight;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, containerBgColor);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(containerPadding, containerPadding));

        ImGui::BeginChild(
            "worlds_container",
            ImVec2(0, childHeight),
            ImGuiChildFlags_None,
            ImGuiWindowFlags_AlwaysUseWindowPadding
        );

        for (const auto& worldName : worlds)
        {
            ImGui::PushID(worldName.c_str());
            ImGui::PushStyleColor(ImGuiCol_ChildBg, entryBgColor);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, childRounding);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(entryPadding, entryPadding));

            ImGui::BeginChild(
                "world_entry",
                ImVec2(0, 0),
                ImGuiChildFlags_AutoResizeY,
                ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar
            );

            ImGui::Text("%s", worldName.c_str());
            ImGui::SameLine();

            float availableWidth = ImGui::GetContentRegionAvail().x;
            float cursorPosX = ImGui::GetCursorPosX() + availableWidth - buttonWidth;
            ImGui::SetCursorPosX(cursorPosX);

            if (ImGui::Button("Load", buttonSize))
            {
                if (isHostingMenu)
                {
                    if (Game::HostWorld(worldName, 33333, 32, true))
                    {
                        if (Game::JoinWorld("dippity", "127.0.0.1", 33333))
                            Engine::Scenes::RemoveAllScenes();
                    }
                }
                else
                {
                    if (Game::LoadWorld(worldName, "dippity"))
                        Engine::Scenes::RemoveAllScenes();
                }
            }

            ImGui::SameLine();
            availableWidth = ImGui::GetContentRegionAvail().x;
            cursorPosX = ImGui::GetCursorPosX() + availableWidth - buttonWidth * deleteButtonOffsetMultiplier;
            ImGui::SetCursorPosX(cursorPosX);

            if (ImGui::Button("Delete", buttonSize))
            {
                DeleteWorld(worldName);
            }

            ImGui::EndChild();

            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor();
            ImGui::PopID();
            ImGui::Spacing();
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        if (ImGui::Button("Back", buttonSize))
        {
            Engine::Scenes::LoadScene(prevMenu);
        }

        ImGui::SameLine();

        const float createButtonWidth = 96.0f;
        ImVec2 createButtonSize(createButtonWidth, buttonHeight);

        float availableWidth = ImGui::GetContentRegionAvail().x;
        float cursorPosX = ImGui::GetCursorPosX() + availableWidth - createButtonWidth;
        ImGui::SetCursorPosX(cursorPosX);

        if (ImGui::Button("Create World", createButtonSize))
        {
            WorldCreationMenu* worldCreationMenu = dynamic_cast<WorldCreationMenu*>(Engine::Scenes::GetScene("world_creation_menu"));

            if (isHostingMenu)
                worldCreationMenu->prevMenu = "worlds_menu_multiplayer";
            else
                worldCreationMenu->prevMenu = "worlds_menu_singleplayer";

            Engine::Scenes::LoadScene("world_creation_menu");
        }

        ImGui::End();
    }

    // bool WorldsMenu::LoadWorld(const std::string& worldName)
    // {
    //     if (worldName.empty())
    //     {
    //         return false;
    //     }
    //     if (!std::filesystem::exists("worlds/" + worldName))
    //     {
    //         return false;
    //     }
    //
    //     Engine::Scenes::RemoveAllScenes();
    //     Scene* worldScene = Engine::Scenes::CreateScene(new World(worldName), "world_" + worldName);
    //     Engine::Scenes::LoadScene(worldScene);
    //
    //     return true;
    // }

    bool WorldsMenu::DeleteWorld(const std::string& worldName)
    {
        if (worldName.empty())
        {
            return false;
        }
        if (!std::filesystem::exists("worlds/" + worldName))
        {
            return false;
        }

        std::filesystem::remove_all("worlds/" + worldName);
        int worldsCount = worlds.size();
        for (int i = 0; i < worldsCount; i++)
        {
            if (worlds[i] == worldName)
                worlds.erase(worlds.begin() + i);
        }

        return true;
    }
}
