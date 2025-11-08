#include "server/Server.hpp"

#include <iostream>

#include "dapper2d/Networking.hpp"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"

namespace Server
{
    bool Server::running = true;
    std::string Server::worldsPathAbs{""};

    bool Server::Init(const int port, const std::string& _worldName, size_t peerCount, bool localOnly)
    {
        Engine::Networking::Init();

        if (!Engine::Networking::CreateServer(port, peerCount, localOnly))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create server.");
            Stop();
            return false;
        }

        Engine::Networking::StartServerThread();

        worldsPathAbs = SDL_GetBasePath() + worldsPath;
        worldData = std::make_unique<WorldData>(_worldName);

        SDL_Log("Server started:");
        SDL_Log("  Port:        %d", port);
        SDL_Log("  Peer count:  %zu", peerCount);
        SDL_Log("  Local only:  %s", localOnly ? "true" : "false");

        return true;
    }

    void Server::Update()
    {
        using clock = std::chrono::steady_clock;
        const std::chrono::milliseconds tickRate(50);

        while (running)
        {
            auto start = clock::now();

            while (const auto& msg = Engine::Networking::PollServerMessage())
            {
                std::string text(msg->data.begin(), msg->data.end());
                SDL_Log("[Server] Recieved : %s", text.c_str());

                std::string message = "Hey client.";
                std::vector<uint8_t> data(message.begin(), message.end());

                Engine::Networking::ServerBroadcast(data, 0);
            }

            auto end = clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            if (elapsed < tickRate)
            {
                SDL_Delay(static_cast<Uint32>((tickRate - elapsed).count()));
            }
        }
    }

    void Server::Clean()
    {
        Engine::Networking::Clean();
    }

    void Server::Stop()
    {
        running = false;
    }
}
