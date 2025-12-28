#pragma once

#include <memory>
#include <string>
#include <vector>

#include "game/client/World.hpp"
#include "game/server/World.hpp"

#include "SDL3/SDL_events.h"

namespace Game
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

        std::unique_ptr<Server::World> serverWorld = nullptr;
        std::unique_ptr<Client::World> clientWorld = nullptr;

        bool disconnectRequested = false;

    public:
        void Init();
        void Update(float delta);
        void Render();
        void HandleEvents(SDL_Event& event);
        void Clean();

        void SetServer(Server::Server* _server, const std::string& worldName);
        void SetClient(Client::Client* _client);

        void OnDisconnectAcknowledged(const std::vector<uint8_t>& data);
    };
}
