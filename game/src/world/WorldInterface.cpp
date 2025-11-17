#include "game/world/WorldInterface.hpp"

#include "dapper2d/Scenes.hpp"
#include "game/game/Game.hpp"
#include "game/menus/MainMenu.hpp"
#include "game/client/WorldClient.hpp"
#include "game/server/WorldServer.hpp"

namespace Game
{
    void WorldInterface::Init()
    {
    }

    void WorldInterface::Update(float delta)
    {
        if (serverWorld)
            serverWorld->Update(delta);
        if (clientWorld)
            clientWorld->Update(delta);
    }

    void WorldInterface::Render()
    {
        if (clientWorld)
            clientWorld->Render();
    }

    void WorldInterface::HandleEvents(SDL_Event& event)
    {
        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            if (event.key.key == SDLK_ESCAPE)
            {
                if (!disconnectRequested && client)
                {
                    std::vector<uint8_t> request;
                    request.push_back(DISCONNECTION_REQUEST);

                    PackBytes(request, &client->clientId, sizeof(uint32_t));

                    client->netInterface->SendToServer(request);
                    disconnectRequested = true;
                }
            }
        }
        if (serverWorld)
            serverWorld->HandleEvents(event);
        if (clientWorld)
            clientWorld->HandleEvents(event);
    }

    void WorldInterface::Clean()
    {
        if (serverWorld)
            serverWorld->Clean();
        if (clientWorld)
            clientWorld->Clean();
    }

    void WorldInterface::SetServer(Server* _server, const std::string& worldName)
    {
        server = _server;
        serverWorld = std::make_unique<WorldServer>(server, worldName);
        serverWorld->Init();
    }

    void WorldInterface::SetClient(Client* _client)
    {
        client = _client;
        clientWorld = std::make_unique<WorldClient>(client);
        clientWorld->Init();
        client->ConnectToEvent(DISCONNECTION_ACKNOWLEDGED, this, &WorldInterface::OnDisconnectAcknowledged);
        client->ConnectToEvent(DISCONNECTION_REQUEST_, this, &WorldInterface::OnDisconnectAcknowledged);
    }

    void WorldInterface::OnDisconnectAcknowledged(const std::vector<uint8_t>& data)
    {
        Game::Disconnect();

        if (!Engine::Scenes::SceneExists("main_menu"))
            Engine::Scenes::CreateScene(new MainMenu(), "main_menu");

        Engine::Scenes::LoadScene("main_menu");
    }
}
