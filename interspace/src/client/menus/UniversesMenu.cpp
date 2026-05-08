#include "interspace/client/menus/UniversesMenu.hpp"

#include <fstream>
#include <filesystem>

#include "imgui.h"
#include "igneous/engine/CFGParser.hpp"
#include "igneous/engine/Database.hpp"

#include "igneous/scenes/SceneManager.hpp"
#include "igneous/engine/Vec2.hpp"
#include "igneous/rendering/Window.hpp"
#include "interspace/shared/game/Game.hpp"
#include "interspace/client/sounds/SoundManager.hpp"

#include "interspace/client/menus/UniverseCreationMenu.hpp"
#include "interspace/shared/datahelpers/UniverseUtils.hpp"
#include "interspace/shared/network/NetworkManager.hpp"
#include "interspace/shared/world/UniverseManager.hpp"

namespace Interspace
{
    void UniversesMenu::OnCreated()
    {
    }

    void UniversesMenu::OnActiveChanged(bool value)
    {
        if (!value)
            return;

        universes.clear();
        for (const auto& [id, name]: UniverseUtils::GetUniverses())
        {
            universes.push_back({name});
        }
    }

    void UniversesMenu::HandleInputs(Engine::InputLayer& layer)
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

        ImGui::Begin("universes", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

        float footerHeight = buttonHeight + ImGui::GetStyle().ItemSpacing.y * 2;
        float childHeight = ImGui::GetContentRegionAvail().y - footerHeight;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, containerBgColor);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(containerPadding, containerPadding));

        ImGui::BeginChild(
                "universes_container",
                ImVec2(0, childHeight),
                ImGuiChildFlags_None,
                ImGuiWindowFlags_AlwaysUseWindowPadding);

        for (const auto& universeEntry: universes)
        {
            ImGui::PushID(universeEntry.name.c_str());
            ImGui::PushStyleColor(ImGuiCol_ChildBg, entryBgColor);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, childRounding);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(entryPadding, entryPadding));

            ImGui::BeginChild(
                    "universe_entry",
                    ImVec2(0, 0),
                    ImGuiChildFlags_AutoResizeY,
                    ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);

            ImGui::Text("%s", universeEntry.name.c_str());
            ImGui::SameLine();

            float availableWidth = ImGui::GetContentRegionAvail().x;
            float cursorPosX = ImGui::GetCursorPosX() + availableWidth - buttonWidth;
            ImGui::SetCursorPosX(cursorPosX);

            if (isHostingMenu)
            {
                if (ImGui::Button("Load", buttonSize))
                {
                    SoundManager::PlaySound("button_1", 1.0f);
                    NetworkManager::CreateRemoteClientServer(32, false);
                    root->RemoveScenes("main_menus");
                    root->AddScene<UniverseManager>("universe_manager", "universe", true);
                }
            }
            else
            {
                if (ImGui::Button("Load", buttonSize))
                {
                    SoundManager::PlaySound("button_1", 1.0f);
                    NetworkManager::CreateLocalClientServer();
                    root->RemoveScenes("main_menus");
                    root->AddScene<UniverseManager>("universe_manager", "universe", true);
                }
            }
            ImGui::SameLine();
            availableWidth = ImGui::GetContentRegionAvail().x;
            cursorPosX = ImGui::GetCursorPosX() + availableWidth - buttonWidth * deleteButtonOffsetMultiplier;
            ImGui::SetCursorPosX(cursorPosX);

            if (ImGui::Button("Delete", buttonSize))
            {
                SoundManager::PlaySound("button_1", 1.0f);
                DeleteUniverse(universeEntry.name);
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
            SoundManager::PlaySound("button_back", 1.0f);
            root->LoadScene(prevMenu);
        }

        ImGui::SameLine();

        const float createButtonWidth = 112.0f;
        ImVec2 createButtonSize(createButtonWidth, buttonHeight);

        float availableWidth = ImGui::GetContentRegionAvail().x;
        float cursorPosX = ImGui::GetCursorPosX() + availableWidth - createButtonWidth;
        ImGui::SetCursorPosX(cursorPosX);

        if (ImGui::Button("Create Universe", createButtonSize))
        {
            SoundManager::PlaySound("button_1", 1.0f);
            UniverseCreationMenu* universeCreationMenu = dynamic_cast<UniverseCreationMenu*>(root->GetScene("universe_creation_menu"));

            if (isHostingMenu)
                universeCreationMenu->prevMenu = "universes_menu_multiplayer";
            else
                universeCreationMenu->prevMenu = "universes_menu_singleplayer";

            root->LoadScene("universe_creation_menu");
        }

        ImGui::End();
    }

    bool UniversesMenu::DeleteUniverse(const std::string& universeName)
    {
        if (universeName.empty())
        {
            return false;
        }
        uint32_t universeId = UniverseUtils::GetUniverseId(universeName);
        UniverseUtils::DeleteUniverse(universeName);
        UniverseUtils::RemoveUniverse(universeId);

        int worldsCount = universes.size();
        for (int i = 0; i < worldsCount; i++)
        {
            if (universes[i].name == universeName)
                universes.erase(universes.begin() + i);
        }

        return true;
    }
}
