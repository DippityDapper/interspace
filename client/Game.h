#ifndef SDL3_FIRST_PROJECT_GAME_H
#define SDL3_FIRST_PROJECT_GAME_H

#include "../common/Common.h"
#include "../common/Packets.h"
#include <map>
#include "ResourceLoader.h"
#include "ClientEntity.h"
#include "../common/Grid.h"

class Game
{
private:
    bool running = true;
    AppState* state = nullptr;

    int clientId = -1;
    ENetHost* client = nullptr;
    ENetPeer* server = nullptr;

    std::map<int, ClientEntity*> entities = {};

private:
    SDL_AppResult InitSDL();
    SDL_AppResult InitEnet();

public:
    Grid grid{};

public:
    void Init();
    void Run();
    void Quit();

    void Event(SDL_Event sdlEvent);
    void EnetEvent(ENetEvent enetEvent);

    SDL_AppResult CreateClient();
    SDL_AppResult ConnectToServer(int port, const char* ip);

    void PacketReceived(ENetEvent enetEvent);

    bool IsRunning() const;
};


#endif