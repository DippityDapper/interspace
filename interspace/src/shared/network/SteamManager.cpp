#include "interspace/shared/network/SteamManager.hpp"

#include "igneous/engine/Engine.hpp"

#include "steam/steam_api.h"
#include "steam/steam_gameserver.h"

namespace Interspace
{
    void SteamManager::OnCreated()
    {
        if (SteamAPI_RestartAppIfNecessary(480))
        {
            Engine::Engine::Quit();
            return;
        }
        TryConnect();
    }

    void SteamManager::Update(double delta)
    {
        RunCallbacks();
    }

    void SteamManager::OnDestroyed()
    {
        Clean();

        if (initialized)
        {
            SteamAPI_Shutdown();
            initialized = false;
        }
    }

    void SteamManager::Clean()
    {
        if (gameServerInitialized)
        {
            SteamGameServer()->LogOff();

            SteamGameServer_Shutdown();

            gameServerInitialized = false;
        }
    }

    bool SteamManager::TryConnect()
    {
        if (initialized)
            return true;

        SteamErrMsg errMsg{};

        ESteamAPIInitResult result = SteamAPI_InitEx(&errMsg);

        if (result != k_ESteamAPIInitResult_OK)
        {
            SDL_Log("Steam failed to initialize: %s", errMsg);
            return false;
        }

        initialized = true;

        SDL_Log("Steam initialized successfully.");

        return true;
    }

    bool SteamManager::InitGameServer(uint16 port)
    {
        if (gameServerInitialized)
            return true;

        bool success = SteamGameServer_Init(
                0,
                port,
                port,
                eServerModeNoAuthentication,
                "1.0.0.0");

        if (!success)
        {
            SDL_Log("SteamGameServer failed to initialize.");
            return false;
        }

        ISteamGameServer* server = SteamGameServer();

        server->SetProduct("Interspace");
        server->SetGameDescription("Interspace");
        server->LogOnAnonymous();

        gameServerInitialized = true;

        SDL_Log("SteamGameServer initialized.");

        return true;
    }

    void SteamManager::RunCallbacks()
    {
        if (initialized)
            SteamAPI_RunCallbacks();

        if (gameServerInitialized)
            SteamGameServer_RunCallbacks();
    }

    bool SteamManager::IsInitialized()
    {
        return initialized;
    }

    bool SteamManager::IsGameServerInitialized()
    {
        return gameServerInitialized;
    }
}