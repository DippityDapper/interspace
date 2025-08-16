#ifndef SDL3_FIRST_PROJECT_GAME_H
#define SDL3_FIRST_PROJECT_GAME_H

#include "../common/Common.h"
#include "../common/Packets.h"
#include <map>
#include "ResourceLoader.h"
#include "ClientEntity.h"

class Game
{
private:
    bool running = true;
    AppState* state;

    ENetHost* client;
    ENetPeer* host;

    bool inputState[4] = {false, false, false, false}; // up, down, left, right
    bool lastInputState[4] = {false, false, false, false};

    std::map<int, ClientEntity*> entities;

    SDL_AppResult InitRendererAndWindow();
    SDL_AppResult InitEnet();
public:
    void Init();

    void Run();
    void Quit();

    SDL_AppResult CreateClient(int port, const char* ip);
    SDL_AppResult ConnectToServer();
    void SendInputPacket(bool up, bool down, bool left, bool right);

    bool IsRunning();
};


#endif
