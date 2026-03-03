#include "interspace/menus/CreateFactionMenu.hpp"

#include "imgui.h"
#include "igneous/rendering/Window.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/network/NetworkPackets.hpp"
#include "igneous/networking/Serializer.hpp"
#include "SDL3/SDL_log.h"

namespace Interspace
{
    void CreateFactionMenu::Init()
    {
        client = Game::client.get();
        if (!client)
        {
            SDL_Log("[Client] Client is null.");
            return;
        }

        acceptedNetEvent = client->ConnectToEvent(CREATE_FACTION_ACCEPTED, this, &CreateFactionMenu::OnFactionAccepted);
        deniedNetEvent = client->ConnectToEvent(CREATE_FACTION_DENIED, this, &CreateFactionMenu::OnFactionDenied);
    }

    void CreateFactionMenu::Update(float delta)
    {
        if (awaitingResponse && errorMessage != "Taking longer than expected...")
        {
            if (timeoutTimer < timeoutClock)
                timeoutTimer += delta;
            else
            {
                errorMessage = "Taking longer than expected...";
            }
        }
    }

    void CreateFactionMenu::UI()
    {
        ImGui::SetNextWindowPos({Engine::Window::viewport.x / 2.0f, Engine::Window::viewport.y / 2.0f}, 0, {0.5f, 0.5f});

        ImGui::Begin("Create Faction", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

        ImGui::BeginDisabled(awaitingResponse);

        ImGui::InputText("##Faction Name", factionNameLineEdit, sizeof(factionNameLineEdit));
        if (ImGui::Button("Create Faction"))
        {
            if (!awaitingResponse)
            {
                CreateFaction();
            }
        }

        ImGui::EndDisabled();

        if (!errorMessage.empty())
            ImGui::Text("%s", errorMessage.c_str());

        ImGui::End();
    }

    void CreateFactionMenu::Clean()
    {
    }

    bool CreateFactionMenu::CreateFaction()
    {
        if (!client)
        {
            errorMessage = "Error with connection.";
            return false;
        }

        std::string factionName = factionNameLineEdit;
        if (factionName.empty())
        {
            errorMessage = "Faction Name is empty.";
            return false;
        }

        SendCreationRequest(factionName);
        awaitingResponse = true;
        return true;
    }
}
