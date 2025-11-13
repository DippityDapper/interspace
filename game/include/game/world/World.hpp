#pragma once

#include <memory>
#include <string>

#include "game/client/WorldClient.hpp"
#include "game/server/WorldServer.hpp"
#include "SDL3/SDL_events.h"

namespace Game
{
    class Client;
    class Server;

    class World
    {
    private:
        Server* server = nullptr;
        Client* client = nullptr;

        std::unique_ptr<WorldServer> serverWorld = nullptr;
        std::unique_ptr<WorldClient> clientWorld = nullptr;

        bool disconnectRequested = false;

    public:
        void Init();
        void Update(float delta);
        void Render();
        void HandleEvents(SDL_Event& event);
        void Clean();

        void SetServer(Server* _server, const std::string& worldName);
        void SetClient(Client* _client);

        void OnDisconnectAcknowledged(const std::vector<uint8_t>& data);
    };
}
