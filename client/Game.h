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
    AppState* state = nullptr;

    ENetHost* client = nullptr;
    ENetPeer* host = nullptr;

    bool inputState[4] = {false, false, false, false}; // up, down, left, right
    bool lastInputState[4] = {false, false, false, false};

    std::map<int, ClientEntity*> entities = {};

private:
    SDL_AppResult InitSDL();
    SDL_AppResult InitEnet();

public:
    void Init();
    void Run();
    void Quit();

    SDL_AppResult CreateClient();
    SDL_AppResult ConnectToServer(int port, const char* ip);
    void SendInputPacket(bool up, bool down, bool left, bool right);

    bool IsRunning() const;
};


#endif
