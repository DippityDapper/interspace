#include "game/game/Game.hpp"

#include "igneous/ResourceLoader.hpp"
#include "SDL3/SDL_events.h"

#include "igneous/Scenes.hpp"
#include "game/game/Sounds.hpp"

#include "game/menus/MainMenu.hpp"
#include "game/network/MultiplayerInterface.hpp"
#include "game/network/SingleplayerInterface.hpp"
#include "game/world/TileRegistry.hpp"

namespace Game
{
    void Game::Init()
    {
        TileRegistry::Init();
        Sounds::AddSound("button_1", "assets/sounds/buttons/button_1.mp3", 0);
        Sounds::AddSound("button_back", "assets/sounds/buttons/button_back.mp3", 0);

        Engine::ResourceLoader::SetScaleMode(SDL_SCALEMODE_PIXELART);

        if (!Engine::Scenes::SceneExists("main_menu"))
            Engine::Scenes::CreateScene(new MainMenu(), "main_menu");
        Engine::Scenes::LoadScene("main_menu");
    }

    void Game::Update(float delta)
    {
        if (serverNetInterface)
            serverNetInterface->Poll();
        if (clientNetInterface)
            clientNetInterface->Poll();

        if (world)
            world->Update(delta);
    }

    void Game::Render()
    {
        if (world)
            world->Render();
    }

    void Game::HandleEvents(SDL_Event& event)
    {
        if (world)
            world->HandleEvents(event);
    }

    void Game::Clean()
    {
        if (world)
            world->Clean();
    }

    bool Game::HostWorld(const std::string& worldName, int port, int peerCount, bool localOnly)
    {
        serverNetInterface = std::make_unique<MultiplayerInterface>(port, peerCount, localOnly);
        server = std::make_unique<Server::Server>(serverNetInterface.get());

        if (!server->netInterface->Connected())
            return false;

        if (!world)
            world = std::make_unique<WorldInterface>();

        world->SetServer(server.get(), worldName);
        world->Init();

        return true;
    }

    bool Game::JoinWorld(const std::string& username, const std::string& ip, int port)
    {
        clientNetInterface = std::make_unique<MultiplayerInterface>(port, ip);
        client = std::make_unique<Client::Client>(clientNetInterface.get(), username);

        if (!client->netInterface->Connected())
            return false;

        if (!world)
            world = std::make_unique<WorldInterface>();

        world->SetClient(client.get());
        world->Init();

        return true;
    }

    bool Game::LoadWorld(const std::string& worldName, const std::string& username)
    {
        serverNetInterface = std::make_unique<SingleplayerInterface>();
        server = std::make_unique<Server::Server>(serverNetInterface.get());
        client = std::make_unique<Client::Client>(serverNetInterface.get(), username);

        if (!client->netInterface->Connected())
            return false;

        if (!world)
            world = std::make_unique<WorldInterface>();

        world->SetServer(server.get(), worldName);
        world->SetClient(client.get());
        world->Init();

        return true;
    }

    void Game::Disconnect()
    {
        world = nullptr;
        client = nullptr;
        server = nullptr;
        clientNetInterface = nullptr;
        serverNetInterface = nullptr;
    }
}
