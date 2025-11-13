#include "game/world/World.hpp"

#include "dapper2d/Scenes.hpp"
#include "game/game/Game.hpp"
#include "game/menus/MainMenu.hpp"
#include "game/client/WorldClient.hpp"
#include "game/server/WorldServer.hpp"

namespace Game
{
    void World::Init()
    {
    }

    void World::Update(float delta)
    {
        if (serverWorld)
            serverWorld->Update(delta);
        if (clientWorld)
            clientWorld->Update(delta);
    }

    void World::Render()
    {
        if (clientWorld)
            clientWorld->Render();
    }

    void World::HandleEvents(SDL_Event& event)
    {
        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            if (event.key.key == SDLK_ESCAPE)
            {
                if (!disconnectRequested && client)
                {
                    std::vector<uint8_t> request;
                    request.push_back(DISCONNECTION_REQUEST);

                    uint8_t* idBytes = reinterpret_cast<uint8_t*>(&client->clientId);
                    request.insert(request.end(), idBytes, idBytes + sizeof(uint32_t));

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

    void World::Clean()
    {
        if (serverWorld)
            serverWorld->Clean();
        if (clientWorld)
            clientWorld->Clean();
    }

    void World::SetServer(Server* _server, const std::string& worldName)
    {
        server = _server;
        serverWorld = std::make_unique<WorldServer>(server, worldName);
        serverWorld->Init();
    }

    void World::SetClient(Client* _client)
    {
        client = _client;
        clientWorld = std::make_unique<WorldClient>(client);
        clientWorld->Init();
        client->ConnectToEvent(DISCONNECTION_ACKNOWLEDGED, this, &World::OnDisconnectAcknowledged);
    }

    void World::OnDisconnectAcknowledged(const std::vector<uint8_t>& data)
    {
        Game::Disconnect();

        if (!Engine::Scenes::SceneExists("main_menu"))
            Engine::Scenes::CreateScene(new MainMenu(), "main_menu");

        Engine::Scenes::LoadScene("main_menu");
    }
}
