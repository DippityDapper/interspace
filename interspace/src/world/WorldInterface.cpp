#include "interspace/world/WorldInterface.hpp"

#include "interspace/game/Game.hpp"
#include "interspace/client/World.hpp"
#include "interspace/server/World.hpp"

namespace Interspace
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

    void WorldInterface::SetServer(Server::Server* _server, const std::string& worldName)
    {
        server = _server;
        serverWorld = std::make_unique<Server::World>(server, worldName);
        serverWorld->Init();
    }

    void WorldInterface::SetClient(Client::Client* _client)
    {
        client = _client;
        clientWorld = std::make_unique<Client::World>(client);
        clientWorld->Init();
    }
}
