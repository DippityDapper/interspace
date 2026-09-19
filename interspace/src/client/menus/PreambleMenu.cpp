#include "interspace/client/menus/PreambleMenu.hpp"

#include "imgui.h"
#include "igneous/rendering/Window.hpp"
#include "igneous/scenes/SceneRoot.hpp"
#include "interspace/client/sounds/SoundManager.hpp"
#include "interspace/shared/world/UniverseManager.hpp"

#include <fstream>
#include <random>

namespace Interspace
{
    void PreambleMenu::HandleInputs(Engine::InputLayer& layer)
    {
        if (!layer.Is("ui"))
            return;

        ImGui::SetNextWindowPos({Engine::Window::viewport.x / 2.0f, Engine::Window::viewport.y / 2.0f}, 0, {0.5f, 0.5f});

        ImGui::Begin("Universe Creation", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

        ImGui::InputText("Universe Name", usernameLineEdit, sizeof(usernameLineEdit));

        if (ImGui::Button("Submit"))
        {
            SoundManager::PlaySound("button_1", 1.0f);
            std::string username{usernameLineEdit};
            if (!username.empty())
            {
                std::ofstream file("data/shared/local_user.txt");
                std::string usernameField{"username=" + username};
                std::mt19937 gen(std::random_device{}());
                uint64_t newId = 0;
                std::uniform_int_distribution<uint64_t> clientIdDist(1, std::numeric_limits<uint64_t>::max());
                newId = clientIdDist(gen);
                std::string idField{"id=" + std::to_string(newId)};

                file << idField << '\n'
                     << usernameField;
                file.close();
                root->RemoveScene(name);
                UniverseManager::clientUniverse->SendConnectionRequest();
            }
            else
            {
                errorMessage = "Error: Username is empty.";
            }
        }

        if (ImGui::Button("Disconnect"))
        {
            SoundManager::PlaySound("button_back", 1.0f);
        }

        if (!errorMessage.empty())
        {
            ImGui::Text("%s", errorMessage.c_str());
        }
        ImGui::End();
    }
}
