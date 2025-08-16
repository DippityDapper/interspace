#ifndef SDL3_FIRST_PROJECT_SERVER_H
#define SDL3_FIRST_PROJECT_SERVER_H

#include "../common/Common.h"
#include "../common/Packets.h"
#include <vector>
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
    AppState* state = new AppState();

    ENetHost* server;
    std::vector<PeerEntity> peerEntities;
    int nextEntityId = 0;

    SDL_AppResult InitRendererAndWindow();
    SDL_AppResult InitEnet();
public:
    void Init();
    void Run();
    void Quit();

    void BroadcastEntityState(ServerEntity* entity, int entityId);
    SDL_AppResult CreateServer(const char* host);
    void ClientConnected(ENetPeer* peer);

    bool IsRunning();
};


#endif
