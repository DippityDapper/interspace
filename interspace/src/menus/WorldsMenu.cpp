#include "interspace/menus/WorldsMenu.hpp"

#include <fstream>
#include <filesystem>

#include "imgui.h"
#include "igneous/CFGParser.hpp"
#include "igneous/Database.hpp"

#include "igneous/Scenes.hpp"
#include "igneous/Vec2.hpp"
#include "igneous/Window.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/game/Sounds.hpp"

#include "interspace/menus/WorldCreationMenu.hpp"
#include "interspace/game/DBHelper.hpp"

namespace Interspace
{
    void WorldsMenu::Init()
    {
        worlds.clear();

        for (auto rows : Engine::Database::Query(DBHelper::db.get(), "SELECT * FROM world;"))
        {
            std::string worldName{rows["worldId"]};
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
                Sounds::PlaySound("button_1", 1.0f);

                Engine::CFGParser::LoadConfig("game_config.cfg", "game_config");
                std::string username = Engine::CFGParser::GetString("game_config", "username");
                if (isHostingMenu)
                {
                    if (Game::HostWorld(worldName, 33333, 32, true))
                    {
                        if (Game::JoinWorld(username, "127.0.0.1", 33333))
                            Engine::Scenes::RemoveAllScenes();
                    }
                }
                else
                {
                    if (Game::LoadWorld(worldName, username))
                        Engine::Scenes::RemoveAllScenes();
                }
            }

            ImGui::SameLine();
            availableWidth = ImGui::GetContentRegionAvail().x;
            cursorPosX = ImGui::GetCursorPosX() + availableWidth - buttonWidth * deleteButtonOffsetMultiplier;
            ImGui::SetCursorPosX(cursorPosX);

            if (ImGui::Button("Delete", buttonSize))
            {
                Sounds::PlaySound("button_1", 1.0f);
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
            Sounds::PlaySound("button_back", 1.0f);
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
            Sounds::PlaySound("button_1", 1.0f);
            WorldCreationMenu* worldCreationMenu = dynamic_cast<WorldCreationMenu*>(Engine::Scenes::GetScene("world_creation_menu"));

            if (isHostingMenu)
                worldCreationMenu->prevMenu = "worlds_menu_multiplayer";
            else
                worldCreationMenu->prevMenu = "worlds_menu_singleplayer";

            Engine::Scenes::LoadScene("world_creation_menu");
        }

        ImGui::End();
    }

    bool WorldsMenu::DeleteWorld(const std::string& worldName)
    {
        if (worldName.empty())
        {
            return false;
        }

        SQLite::Database* db = DBHelper::db.get();
        auto result = Engine::Database::Query(db, "SELECT * FROM world WHERE worldId = '" + worldName + "'");

        if (result.empty())
        {
            return false;
        }

        Engine::Database::Execute(db, "DELETE FROM world WHERE worldId = '" + worldName + "'");

        int worldsCount = worlds.size();
        for (int i = 0; i < worldsCount; i++)
        {
            if (worlds[i] == worldName)
                worlds.erase(worlds.begin() + i);
        }

        return true;
    }
}
