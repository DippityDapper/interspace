#ifndef SDL3_FIRST_PROJECT_GAME_H
#define SDL3_FIRST_PROJECT_GAME_H

#include "../../common/src/Appstate.h"
#include "../../common/src/Packets.h"
#include <map>
#include "ResourceLoader.h"
#include "ClientEntity.h"
#include "../../common/src/Grid.h"
#include "NetworkManager.h"
#include "World.h"

class Game
{
private:
    bool running = true;
    NetworkManager networkManager{};

    World world{};

public:
    static Appstate* state;

private:
    SDL_AppResult InitSDL();
    SDL_AppResult InitNetworking();

public:
    void Init();
    void Update();
    void Render();
    void Clean();
    void HandleEvents();
    bool IsRunning() const;
};


#endif