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

    int clientId = -1;
    ENetHost* client = nullptr;
    ENetPeer* server = nullptr;

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

    void Event(SDL_Event sdlEvent);
    void EnetEvent(ENetEvent enetEvent);

    SDL_AppResult CreateClient();
    SDL_AppResult ConnectToServer(int port, const char* ip);
    void SendInputPacket(bool up, bool down, bool left, bool right);

    void PacketReceived(ENetEvent enetEvent);

    bool IsRunning() const;
};


#endif
