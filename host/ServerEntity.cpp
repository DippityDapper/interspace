#include "ServerEntity.h"

ServerEntity::ServerEntity(float x, float y)
{
    position.y = y;
    position.x = x;
}

void ServerEntity::Update(float delta)
{
    Position dir{0, 0};

    if (inputs[UP])
    {
        dir.y -= 1;
    }
    if (inputs[DOWN])
    {
        dir.y += 1;
    }
    if (inputs[LEFT])
    {
        dir.x -= 1;
    }
    if (inputs[RIGHT])
    {
        dir.x += 1;
    }

    position += dir.Normalized() * speed * delta;
}

Position ServerEntity::GetPosition()
{
    return position;
}

void ServerEntity::SetInputs(bool up, bool down, bool left, bool right)
{
    inputs[UP] = up;
    inputs[DOWN] = down;
    inputs[LEFT] = left;
    inputs[RIGHT] = right;
}
