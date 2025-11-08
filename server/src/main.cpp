#include <string>

#include "SDL3/SDL_log.h"

#include "server/Server.hpp"

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        SDL_Log("Usage: %s <port> <world-name> [--peer-count N] [--local-only]", argv[0]);
        SDL_Log("Example: %s 7777 cool-world --peer-count 64 --local-only", argv[0]);
        return 1;
    }

    int port = -1;
    try
    {
        port = std::stoi(argv[1]);
        if (port <= 0 || port > 65535)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Invalid port number: %d (must be 1–65535)", port);
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Port must be an integer. %s", e.what());
        return 1;
    }

    std::string worldName = argv[2];

    size_t peerCount = 32;
    bool localOnly = true;

    for (int i = 3; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--peer-count" && i + 1 < argc)
        {
            try
            {
                peerCount = std::stoul(argv[++i]);
            }
            catch (const std::exception& e)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Invalid peer count: %s", e.what());
                return 1;
            }
        }
        else if (arg == "--local-only")
        {
            localOnly = true;
        }
        else
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unknown argument: %s", arg.c_str());
            SDL_Log("Usage: %s <port> [--peer-count N] [--local-only]", argv[0]);
            return 1;
        }
    }

    std::unique_ptr<Server::Server> server = std::make_unique<Server::Server>();

    if (!server->Init(port, worldName, peerCount, localOnly))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Server initialization failed.");
        return 1;
    }

    server->Update();
    server->Clean();
    SDL_Log("Server stopped successfully.");
    return 0;
}