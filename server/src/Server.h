#ifndef SDL3_FIRST_PROJECT_SERVER_H
#define SDL3_FIRST_PROJECT_SERVER_H

#include "../../common/src/Appstate.h"
#include "../../common/src/Packets.h"
#include <thread>
#include <map>
#include "ServerEntity.h"
#include "../../common/src/Grid.h"
#include "NetworkManager.h"
#include "World.h"
#include "GameData.h"


class Server
{
private:
    bool running = true;
    Appstate* state = nullptr;
    std::thread terminalThread;

    NetworkManager networkManager{};
    GameData data{};

    World world{};

private:
    SDL_AppResult InitSDL();
    SDL_AppResult InitNetworking();

public:
    void Init();
    void Update();
    void Clean();
    void HandleEvents();

    void TerminalThread();
    bool IsRunning() const;
};


#endif
