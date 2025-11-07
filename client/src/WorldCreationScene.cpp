#include "client/WorldCreationScene.hpp"

#include <fstream>
#include <filesystem>
#include <random>

#include "SDL3/SDL.h"
#include "imgui.h"

#include "dapper2d/Engine.hpp"
#include "dapper2d/Renderer.hpp"
#include "dapper2d/ResourceLoader.hpp"
#include "dapper2d/Window.hpp"

#include "client/World.hpp"

namespace Game
{
    void WorldCreationScene::Init()
    {
        Engine::ResourceLoader::SetScaleMode(SDL_SCALEMODE_PIXELART);

        camera = std::make_unique<Engine::Camera>
        (
            0,
            0,
            1.0f
        );
    }

    void WorldCreationScene::HandleEvents(SDL_Event &event)
    {
        if (event.key.down)
        {
            if (event.key.key == SDLK_ESCAPE)
            {
                Engine::Engine::Quit();
                return;
            }
        }
    }

    void WorldCreationScene::Update(float delta)
    {

    }

    void WorldCreationScene::Render()
    {
        ImGui::SetNextWindowPos({Engine::Window::viewport.x/2.0f,Engine::Window::viewport.y/2.0f}, 0, {0.5f,0.5f});

        ImGui::Begin
        ("World Creation", NULL,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove
         );

        ImGui::InputText("World Name", worldName, sizeof(worldName));
        ImGui::InputText("World Seed", worldSeed, sizeof(worldSeed));


        ImGui::RadioButton("Small", &worldSize, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Medium", &worldSize, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Large", &worldSize, 2);

        if (ImGui::Button("Load World"))
        {
            LoadWorld();
        }
        ImGui::SameLine();
        if (ImGui::Button("Create World"))
        {
            CreateWorld();
        }

        if (!errorMessage.empty())
        {
            ImGui::Text("%s", errorMessage.c_str());
        }
        ImGui::End();
    }

    bool WorldCreationScene::LoadWorld()
    {
        std::string name{worldName};
        if (name.empty())
        {
            errorMessage = "World name is empty.";
            return false;
        }
        if (!std::filesystem::exists("worlds/" + name))
        {
            errorMessage = "World name does not exist.";
            return false;
        }
        errorMessage = "";
        Engine::Engine::SetScene(new Game::World(name));
        return true;
    }

    bool WorldCreationScene::CreateWorld()
    {
        std::string name{worldName};
        std::string seedStr{worldSeed};

        if (name.empty())
        {
            errorMessage = "World name is empty.";
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
                errorMessage = "World seed must be numerical.";
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
            errorMessage = "World name already exists.";
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

        errorMessage = "";
        configFile.close();
        return true;
    }

    void WorldCreationScene::Clean()
    {

    }
}