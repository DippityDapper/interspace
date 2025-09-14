#ifndef SDL3_FIRST_PROJECT_WORLD_H
#define SDL3_FIRST_PROJECT_WORLD_H


#include <map>
#include "SDL3/SDL.h"
#include "Area.h"

class World
{
public:
    Vec2 currentAreaPosition{0, 0};
    std::map<Vec2, Area> areas{};
    Vec2 worldSize{0, 0};

    std::map<int, std::unique_ptr<ClientEntity>> entities{};

public:
    SDL_AppResult Init(Vec2 _worldSize);
    Area* GetCurrentArea();
    void Render(SDL_Renderer* renderer);
    void RenderEntities(SDL_Renderer *renderer);
    void CleanEntities();

private:
    void HandleDisconnectPacket(ENetEvent& event);
    void HandleStatePacket(ENetEvent& event);
    void HandleCreateClientEntityPacket(ENetEvent& event);
};


#endif
