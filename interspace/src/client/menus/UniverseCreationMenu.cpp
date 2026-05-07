#include "interspace/client/menus/UniverseCreationMenu.hpp"

#include <fstream>
#include <filesystem>
#include <random>

#include "imgui.h"
#include "igneous/engine/Database.hpp"

#include "igneous/scenes/SceneManager.hpp"
#include "igneous/rendering/Window.hpp"
#include "interspace/shared/datahelpers/UniverseManager.hpp"
#include "interspace/shared/datahelpers/WorldManager.hpp"
#include "interspace/shared/game/Game.hpp"
#include "interspace/client/sounds/SoundManager.hpp"

namespace Interspace
{
    void UniverseCreationMenu::OnCreated()
    {
    }

    void UniverseCreationMenu::HandleInputs(Engine::InputLayer& layer)
    {
        if (!layer.Is("ui"))
            return;

        ImGui::SetNextWindowPos({Engine::Window::viewport.x / 2.0f, Engine::Window::viewport.y / 2.0f}, 0, {0.5f, 0.5f});

        ImGui::Begin("Universe Creation", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

        ImGui::InputText("Universe Name", universeNameLineEdit, sizeof(universeNameLineEdit));
        ImGui::InputText("Universe Seed", universeSeedLineEdit, sizeof(universeSeedLineEdit));

        if (ImGui::Button("Create Universe"))
        {
            SoundManager::PlaySound("button_1", 1.0f);
            if (CreateUniverse())
                root->LoadScene(prevMenu);
        }

        if (ImGui::Button("Back"))
        {
            SoundManager::PlaySound("button_back", 1.0f);
            root->LoadScene(prevMenu);
        }

        if (!message.empty())
        {
            ImGui::Text("%s", message.c_str());
        }
        ImGui::End();
    }

    bool UniverseCreationMenu::CreateUniverse()
    {
        std::string name{universeNameLineEdit};
        std::string seedStr{universeSeedLineEdit};

        if (name.empty())
        {
            message = "Universe name is empty.";
            return false;
        }

        std::string serverUniverseDirPath{"data/server/" + name};
        if (std::filesystem::exists(serverUniverseDirPath) && std::filesystem::is_directory(serverUniverseDirPath))
        {
            message = "Universe name already exists.";
            return false;
        }
        std::string clientWorldDirPath{"data/client/" + name};
        if (std::filesystem::exists(clientWorldDirPath) && std::filesystem::is_directory(clientWorldDirPath))
        {
            message = "Universe name already exists.";
            return false;
        }

        uint32_t seed = 0;
        if (seedStr.empty())
        {
            std::mt19937 gen(std::random_device{}());
            std::uniform_int_distribution<client_id_t> seedDist(1, UINT32_MAX);
            seed = seedDist(gen);
        }
        else
        {
            if (seedStr.find('-') != std::string::npos)
            {
                message = "Universe seed must be a positive number.";
                return false;
            }
            try
            {
                seed = std::stoul(seedStr);
            }
            catch (const std::invalid_argument&)
            {
                message = "Universe seed must be a positive number.";
                return false;
            }
        }

        UniverseManager::InsertUniverse(name, seed);
        universe_id_t universeId = UniverseManager::GetUniverseId(name);
        UniverseManager::AddUniverse(name, universeId);
        WorldManager::InsertWorld(name, Test, 512, 512);

        message = "";

        return true;
    }
}
