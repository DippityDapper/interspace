#include "interspace/menus/WorldCreationMenu.hpp"

#include <fstream>
#include <filesystem>
#include <random>

#include "imgui.h"
#include "igneous/engine/Database.hpp"

#include "igneous/scenes/SceneManager.hpp"
#include "igneous/rendering/Window.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/game/Sounds.hpp"

namespace Interspace
{
    void WorldCreationMenu::Init()
    {
    }

    void WorldCreationMenu::Render()
    {
        ImGui::SetNextWindowPos({Engine::Window::viewport.x / 2.0f, Engine::Window::viewport.y / 2.0f}, 0, {0.5f, 0.5f});

        ImGui::Begin("World Creation", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

        ImGui::InputText("World Name", worldNameLineEdit, sizeof(worldNameLineEdit));
        ImGui::InputText("World Seed", worldSeedLineEdit, sizeof(worldSeedLineEdit));

        ImGui::RadioButton("Small", &worldSize, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Medium", &worldSize, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Large", &worldSize, 2);

        if (ImGui::Button("Create World"))
        {
            Sounds::PlaySound("button_1", 1.0f);
            if (CreateWorld())
                root->LoadScene(prevMenu);
        }

        if (ImGui::Button("Back"))
        {
            Sounds::PlaySound("button_back", 1.0f);
            root->LoadScene(prevMenu);
        }

        if (!message.empty())
        {
            ImGui::Text("%s", message.c_str());
        }
        ImGui::End();
    }

    bool WorldCreationMenu::CreateWorld()
    {
        std::string name{worldNameLineEdit};
        std::string seedStr{worldSeedLineEdit};

        if (name.empty())
        {
            message = "World name is empty.";
            return false;
        }

        std::string serverWorldDirPath{"data/server/" + name};
        if (std::filesystem::exists(serverWorldDirPath) && std::filesystem::is_directory(serverWorldDirPath))
        {
            message = "World name already exists.";
            return false;
        }
        std::string clientWorldDirPath{"data/client/" + name};
        if (std::filesystem::exists(clientWorldDirPath) && std::filesystem::is_directory(clientWorldDirPath))
        {
            message = "World name already exists.";
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
                message = "World seed must be a positive number.";
                return false;
            }
            try
            {
                seed = std::stoul(seedStr);
            }
            catch (const std::invalid_argument&)
            {
                message = "World seed must be a positive number.";
                return false;
            }
        }

        uint16_t worldSizeX = 128;
        uint16_t worldSizeY = 128;

        if (worldSize == 0) // small
        {
            worldSizeX = 128;
            worldSizeY = 128;
        }
        else if (worldSize == 1) // medium
        {
            worldSizeX = 512;
            worldSizeY = 512;
        }
        else if (worldSize == 2) // large
        {
            worldSizeX = 1024;
            worldSizeY = 1024;
        }

        DBHelper::CreateWorld(name, seed, worldSizeX, worldSizeY);

        message = "";

        return true;
    }
}
