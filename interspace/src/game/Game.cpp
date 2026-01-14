#include "interspace/game/Game.hpp"

#include "igneous/networking/LocalNetwork.hpp"
#include "igneous/networking/NetworkManager.hpp"
#include "igneous/networking/RemoteNetwork.hpp"
#include "igneous/resources/ResourceManager.hpp"

#include "igneous/scenes/SceneManager.hpp"
#include "interspace/game/Sounds.hpp"

#include "interspace/menus/MainMenu.hpp"
#include "interspace/game/DBHelper.hpp"
#include "interspace/world/TileRegistry.hpp"

namespace Interspace
{
    void Game::Init()
    {
        singleton = true;
        DBHelper::InitDatabase();
        TileRegistry::Init();
        Engine::ResourceManager::SetScaleMode(SDL_SCALEMODE_PIXELART);

        Sounds::AddSound("button_1", "assets/sounds/buttons/button_1.mp3", 0);
        Sounds::AddSound("button_back", "assets/sounds/buttons/button_back.mp3", 0);

        if (!mainMenu)
            mainMenu = root->AddScene<MainMenu>("main_menu", true);
    }

    void Game::Update(float delta)
    {
        if (pendingDisconnect)
        {
            pendingDisconnect = false;
            world->Clean();
            world = nullptr;
            client = nullptr;
            server = nullptr;
            clientNetInterface = nullptr;
            serverNetInterface = nullptr;
        }

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

    void Game::HandleEvents(Engine::InputLayer& layer)
    {
        if (world)
            world->HandleEvents(layer);
    }

    void Game::Clean()
    {
        if (world)
            world->Clean();
    }

    bool Game::HostWorld(const std::string& worldName, int port, int peerCount, bool localOnly)
    {
        serverNetInterface = Engine::NetworkManager::CreateServer(port, peerCount, localOnly);
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
        clientNetInterface = Engine::NetworkManager::CreateClient(port, ip);
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
        Engine::NetworkManager::CreateLocalNetwork(serverNetInterface, clientNetInterface);

        server = std::make_unique<Server::Server>(serverNetInterface.get());
        client = std::make_unique<Client::Client>(clientNetInterface.get(), username);

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
        pendingDisconnect = true;
    }
}
