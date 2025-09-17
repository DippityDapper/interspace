#ifndef SDL3_FIRST_PROJECT_WORLD_H
#define SDL3_FIRST_PROJECT_WORLD_H

#include "SDL3/SDL.h"
#include "Area.h"
#include "ServerEntity.h"
#include "../../common/src/AStar.h"
#include "queue"
#include "NetworkManager.h"

class World
{
public:
    Vec2 currentAreaPosition{0, 0};
    std::map<Vec2, Area> areas{};
    Vec2 worldSize{0, 0};
    AStar::AStar astar{};
    std::map<int, std::unique_ptr<ServerEntity>> entities{};

    std::queue<int> entityPositionUpdated{};

public:
    SDL_AppResult Init(Vec2 _worldSize);
    void Update(float delta);
    void NetworkUpdate(NetworkManager* nm);

    Area* GetCurrentArea();
    void CleanEntities();

    void OnClientConnected(int clientId, NetworkManager*);
    void OnClientDisconnected(int clientId);

private:
    void HandlePositionPacket(ENetEvent &enetEvent);
};


#endif
