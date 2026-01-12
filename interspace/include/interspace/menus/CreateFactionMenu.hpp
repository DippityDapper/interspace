#pragma once

#include <vector>

#include "igneous/scenes/Scene.hpp"
#include "interspace/client/ClientNetEvent.hpp"

namespace Interspace::Client
{
    class Client;
}

namespace Interspace
{
    class CreateFactionMenu : public Engine::Scene
    {
    private:
        Client::Client* client = nullptr;

        Client::ClientNetEvent::Connection acceptedNetEvent{};
        Client::ClientNetEvent::Connection deniedNetEvent{};

        float timeoutClock = 10.0f;
        float timeoutTimer = 0.0f;

        char factionNameLineEdit[32] = "";

        std::string errorMessage{};
        bool awaitingResponse = false;

    private:
        void Init() override;
        void Update(float delta) override;
        void Render() override;
        void Clean() override;

        bool CreateFaction();

        void OnFactionAccepted(const std::vector<uint8_t>& data);
        void OnFactionDenied(const std::vector<uint8_t>& data);
    };
} // Interspace