#ifndef SDL3_FIRST_PROJECT_SERVER_H
#define SDL3_FIRST_PROJECT_SERVER_H

#include "../common/Common.h"
#include "../common/Packets.h"
#include <vector>
#include <thread>
#include "ServerEntity.h"

struct PeerEntity
{
    ServerEntity* entity;
    ENetPeer* peer;
    int entityId;
};

class Server
{
private:
    bool running = true;
    AppState* state = nullptr;

    ENetHost* server = nullptr;
    std::thread terminalThread;
    std::vector<PeerEntity> peerEntities = {};
    int nextEntityId = 0;

    SDL_AppResult InitSDL();
    SDL_AppResult InitEnet();
public:
    void Init();
    void Run();
    void Quit();

    void BroadcastEntityState(ServerEntity* entity, int entityId);
    SDL_AppResult CreateServer(const char* host);
    void ClientConnected(ENetPeer* peer);
    void TerminalThread();

    bool IsRunning() const;
};


#endif
