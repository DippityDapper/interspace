#include "interspace/menus/WorldsMenu.hpp"

#include <fstream>
#include <filesystem>

#include "imgui.h"
#include "igneous/engine/CFGParser.hpp"
#include "igneous/engine/Database.hpp"

#include "igneous/scenes/SceneManager.hpp"
#include "igneous/engine/Vec2.hpp"
#include "igneous/rendering/Window.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/game/Sounds.hpp"

#include "interspace/menus/WorldCreationMenu.hpp"
#include "interspace/game/DBHelper.hpp"

namespace Interspace
{
    void WorldsMenu::Init()
    {
    }

    void WorldsMenu::OnActiveChanged(bool value)
    {
        if (!value)
            return;

        worlds.clear();

        SQLite::Statement statement{*DBHelper::worldsDb, "SELECT worldId FROM world;"};

        while (statement.executeStep())
        {
            std::string worldName{statement.getColumn(0).getString()};
            worlds.push_back({worldName});
        }
    }

    void WorldsMenu::UI(Engine::InputLayer& layer)
    {
        if (!layer.Is("ui"))
            return;

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

        ImGui::Begin("Worlds", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

        float footerHeight = buttonHeight + ImGui::GetStyle().ItemSpacing.y * 2;
        float childHeight = ImGui::GetContentRegionAvail().y - footerHeight;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, containerBgColor);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(containerPadding, containerPadding));

        ImGui::BeginChild(
                "worlds_container",
                ImVec2(0, childHeight),
                ImGuiChildFlags_None,
                ImGuiWindowFlags_AlwaysUseWindowPadding);

        for (const auto& worldEntry: worlds)
        {
            ImGui::PushID(worldEntry.name.c_str());
            ImGui::PushStyleColor(ImGuiCol_ChildBg, entryBgColor);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, childRounding);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(entryPadding, entryPadding));

            ImGui::BeginChild(
                    "world_entry",
                    ImVec2(0, 0),
                    ImGuiChildFlags_AutoResizeY,
                    ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);

            ImGui::Text("%s", worldEntry.name.c_str());
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
                    if (Game::HostWorld(worldEntry.name, 33333, 32, true))
                    {
                        Game::JoinWorld(username, "127.0.0.1", 33333);
                    }
                }
                else
                {
                    Game::LoadWorld(worldEntry.name, username);
                }
            }

            ImGui::SameLine();
            availableWidth = ImGui::GetContentRegionAvail().x;
            cursorPosX = ImGui::GetCursorPosX() + availableWidth - buttonWidth * deleteButtonOffsetMultiplier;
            ImGui::SetCursorPosX(cursorPosX);

            if (ImGui::Button("Delete", buttonSize))
            {
                Sounds::PlaySound("button_1", 1.0f);
                DeleteWorld(worldEntry.name);
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
            root->LoadScene(prevMenu);
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
            WorldCreationMenu* worldCreationMenu = dynamic_cast<WorldCreationMenu*>(root->GetScene("world_creation_menu"));

            if (isHostingMenu)
                worldCreationMenu->prevMenu = "worlds_menu_multiplayer";
            else
                worldCreationMenu->prevMenu = "worlds_menu_singleplayer";

            root->LoadScene("world_creation_menu");
        }

        ImGui::End();
    }

    bool WorldsMenu::DeleteWorld(const std::string& worldName)
    {
        if (worldName.empty())
        {
            return false;
        }
        DBHelper::DeleteWorld(worldName);

        int worldsCount = worlds.size();
        for (int i = 0; i < worldsCount; i++)
        {
            if (worlds[i].name == worldName)
                worlds.erase(worlds.begin() + i);
        }

        return true;
    }
}
