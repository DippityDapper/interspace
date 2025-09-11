#ifndef SDL3_FIRST_PROJECT_SERVERENTITY_H
#define SDL3_FIRST_PROJECT_SERVERENTITY_H

#include <vector>
#include "SDL3/SDL.h"
#include "../common/Position.h"

enum Directions
{
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
};

class ServerEntity
{
private:
    Position position;
    bool inputs[4] = {false, false, false, false};

    float speed = 300;
    std::vector<Position> path{};
    Position target{};
    bool isPathing = false;
public:
    ServerEntity(float x, float y);
    void Update(float delta);
    void SetPosition(float x, float y);
    Position GetPosition();
    void SetPath(const std::vector<Position>& newPath);
};

#endif
