#ifndef SDL3_FIRST_PROJECT_SERVER_H
#define SDL3_FIRST_PROJECT_SERVER_H

#include "../common/Common.h"
#include "../common/Packets.h"
#include <thread>
#include <map>
#include "ServerEntity.h"

struct PeerEntity
{
    ServerEntity* entity;
    ENetPeer* peer;
};

class Server
{
private:
    bool running = true;
    AppState* state = nullptr;

    ENetHost* server = nullptr;
    std::thread terminalThread;

    std::map<int, ENetPeer*> peers = {};
    std::map<int, PeerEntity> peerEntities = {};
    int nextEntityId = 0;

    SDL_AppResult InitSDL();
    SDL_AppResult InitEnet();
public:
    void Init();
    void Run();
    void Quit();

    void Event(SDL_Event event);
    void EnetEvent(ENetEvent enetEvent);

    void BroadcastEntityState(ServerEntity* entity, int clientId);
    SDL_AppResult CreateServer(const char* host);
    void TerminalThread();

    void RemovePeer(int clientId);

    void DisconnectClient(int clientId);

    void PacketReceived(ENetEvent enetEvent);

    bool IsRunning() const;
};


#endif
