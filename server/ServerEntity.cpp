#include "ServerEntity.h"

ServerEntity::ServerEntity(float x, float y)
{
    position.y = y;
    position.x = x;
}

void ServerEntity::Update(float delta)
{
    if (!isPathing && !path.empty())
    {
        isPathing = true;
        target = path.front() * 64 + Position{32, 32};

        path.erase(path.begin());
    }
    if (isPathing)
    {
        Position direction = (target - position).Normalized();
        position += direction * speed * delta;

        if (position.DistanceTo(target) < 5)
        {
            isPathing = false;
            position = target;
        }
    }
}

void ServerEntity::SetPosition(float x, float y)
{
    position.x = x;
    position.y = y;
}

Position ServerEntity::GetPosition()
{
    return position;
}

void ServerEntity::SetPath(const std::vector<Position>& newPath)
{
    path.clear();
    for (Position p : newPath)
    {
        path.push_back(p);
    }
}
