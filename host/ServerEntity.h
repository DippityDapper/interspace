#ifndef SDL3_FIRST_PROJECT_SERVERENTITY_H
#define SDL3_FIRST_PROJECT_SERVERENTITY_H

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

    float speed = 500;
public:
    ServerEntity(float x, float y);
    void Update(float delta);
    void SetPosition(int x, int y);
    Position GetPosition();
    void SetInputs(bool up, bool down, bool left, bool right);
};

#endif
