#include "game/menus/WorldCreationMenu.hpp"

#include <fstream>
#include <filesystem>
#include <random>

#include "SDL3/SDL.h"
#include "imgui.h"

#include "dapper2d/Networking.hpp"
#include "dapper2d/Scenes.hpp"
#include "dapper2d/ResourceLoader.hpp"
#include "dapper2d/Window.hpp"

#include "game/menus/MainMenu.hpp"
#include "game/menus/WorldsMenu.hpp"

namespace Game
{
    void WorldCreationMenu::Init()
    {
    }

    void WorldCreationMenu::Render()
    {
        ImGui::SetNextWindowPos({Engine::Window::viewport.x/2.0f,Engine::Window::viewport.y/2.0f}, 0, {0.5f,0.5f});

        ImGui::Begin
        ("World Creation", nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove
         );

        ImGui::InputText("World Name", worldNameLineEdit, sizeof(worldNameLineEdit));
        ImGui::InputText("World Seed", worldSeedLineEdit, sizeof(worldSeedLineEdit));

        ImGui::RadioButton("Small", &worldSize, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Medium", &worldSize, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Large", &worldSize, 2);

        if (ImGui::Button("Create World"))
        {
            CreateWorld();
            ImGui::End();
            return;
        }

        if (ImGui::Button("Back"))
        {
            Engine::Scenes::LoadScene(prevMenu);
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

        uint32_t seed = 0;
        if (seedStr.empty())
        {
            std::mt19937 gen(std::random_device{}());
            std::uniform_int_distribution<> seedDist(0, UINT32_MAX);
            seed = seedDist(gen);
        }
        else
        {
            try
            {
                seed = std::stoul(seedStr);
            }
            catch (const std::invalid_argument&)
            {
                message = "World seed must be numerical.";
                return false;
            }
        }

        if (!std::filesystem::exists("worlds"))
        {
            std::filesystem::create_directories("worlds");
            std::filesystem::create_directories("worlds/" + name);
            std::filesystem::create_directories("worlds/" + name + "/regions");
        }
        else if (!std::filesystem::exists("worlds/" + name))
        {
            std::filesystem::create_directories("worlds/" + name);
            std::filesystem::create_directories("worlds/" + name + "/regions");
        }
        else if (!std::filesystem::exists("worlds/" + name + "/regions"))
        {
            std::filesystem::create_directories("worlds/" + name + "/regions");
        }
        else
        {
            message = "World name already exists.";
            return false;
        }

        std::fstream configFile("worlds/" + name + "/configs.cfg", std::ios::out);

        std::string finalSeedStr{"world_seed=" + std::to_string(seed) + "\n"};
        configFile.write(finalSeedStr.c_str(), finalSeedStr.size());

        std::string areaSizeStr{"area_size=64\n"};
        configFile.write(areaSizeStr.c_str(), areaSizeStr.size());

        std::string tileSizeStr{"tile_size=32\n"};
        configFile.write(tileSizeStr.c_str(), tileSizeStr.size());

        if (worldSize == 0) // small
        {
            std::string worldSizeXStr{"world_size_x=128\n"};
            configFile.write(worldSizeXStr.c_str(), worldSizeXStr.size());
            std::string worldSizeYStr{"world_size_y=128\n"};
            configFile.write(worldSizeYStr.c_str(), worldSizeYStr.size());
        }
        else if (worldSize == 1) // med
        {
            std::string worldSizeXStr{"world_size_x=512\n"};
            configFile.write(worldSizeXStr.c_str(), worldSizeXStr.size());
            std::string worldSizeYStr{"world_size_y=512\n"};
            configFile.write(worldSizeYStr.c_str(), worldSizeYStr.size());
        }
        else if (worldSize == 2) //large
        {
            std::string worldSizeXStr{"world_size_x=1024\n"};
            configFile.write(worldSizeXStr.c_str(), worldSizeXStr.size());
            std::string worldSizeYStr{"world_size_y=1024\n"};
            configFile.write(worldSizeYStr.c_str(), worldSizeYStr.size());
        }

        std::string regionSize{"region_size=32\n"};
        configFile.write(regionSize.c_str(), regionSize.size());

        configFile.close();
        message = "";

        if (!Engine::Scenes::SceneExists("worlds_menu"))
        {
            WorldsMenu* worldsScene = new WorldsMenu();
            Engine::Scenes::CreateScene(worldsScene, "worlds_menu");
        }
        Engine::Scenes::LoadScene("worlds_menu");

        return true;
    }

    bool WorldCreationMenu::HostWorld()
    {
        std::string name{worldNameLineEdit};

        if (name.empty())
        {
            message = "World name is empty.";
            return false;
        }

        if (!Engine::Networking::CreateServer(33333, 3, true))
        {
            Engine::Networking::StopServer();
            message = "Failed to create server process.";
            return false;
        }

        message = "Successfully hosted world " + name + ".";
        return true;
    }

    bool WorldCreationMenu::StopHostingWorld()
    {
        if (!Engine::Networking::StopServer())
        {
            message = "No server running.";
            return false;
        }

        message = "Stopped hosting.";
        return true;
    }

    bool WorldCreationMenu::ConnectToWorld()
    {
        Engine::Networking::StopClientThread();

        if (!Engine::Networking::ConnectToServer("127.0.0.1", 33333))
        {
            message = "Failed to connect to server.";
            return false;
        }

        Engine::Networking::StartClientThread();
        message = "Connected to server.";
        return true;
    }

    bool WorldCreationMenu::DisconnectFromWorld()
    {
        if (!Engine::Networking::DisconnectFromServer())
        {
            message = "Not connected to a server.";
            return false;
        }

        Engine::Networking::StopClientThread();
        message = "Disconnected from server.";
        return true;
    }
}
