#ifndef SDL3_FIRST_PROJECT_NETWORKMANAGER_H
#define SDL3_FIRST_PROJECT_NETWORKMANAGER_H

#include "SDL3/SDL.h"
#include "enet/enet.h"
#include "../../common/Packets.h"
#include <map>
#include <functional>
using PacketHandler = std::function<void(ENetEvent&)>;

class NetworkManager
{
public:
    int clientId = -1;
    ENetHost* client = nullptr;
    ENetPeer* server = nullptr;
    static std::map<PacketType, PacketHandler> packetHandlers;

public:
    SDL_AppResult Init();
    void InitPacketHandlers();

    SDL_AppResult CreateClient();
    SDL_AppResult ConnectToServer(int port, const char* ip);
    void Disconnect() const;

    bool HandleNetworkEvents();
    bool HandlePackets(ENetEvent enetEvent);

    void HandleClientDataPacket(ENetEvent& event);
private:

};


#endif
