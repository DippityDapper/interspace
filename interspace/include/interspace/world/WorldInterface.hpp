#pragma once

#include <memory>
#include <string>
#include <vector>

#include "interspace/client/World.hpp"
#include "interspace/server/World.hpp"

#include "SDL3/SDL_events.h"

namespace Interspace
{
    namespace Client
    {
        class Client;
    }

    namespace Server
    {
        class Server;
    }

    class WorldInterface
    {
    private:
        Server::Server* server = nullptr;
        Client::Client* client = nullptr;

    public:
        std::unique_ptr<Server::World> serverWorld = nullptr;
        std::unique_ptr<Client::World> clientWorld = nullptr;

    public:
        void Init();
        void Update(float delta);
        void Render();
        void HandleEvents(SDL_Event& event);
        void Clean();

        void SetServer(Server::Server* _server, const std::string& worldName);
        void SetClient(Client::Client* _client);
    };
}
