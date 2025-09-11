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
        target = path.front();

        path.erase(path.begin());
    }
    if (isPathing)
    {
        Vec2 direction = (target - position).Normalized();
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

Vec2 ServerEntity::GetPosition()
{
    return position;
}

void ServerEntity::SetPath(const std::vector<Vec2>& newPath)
{
    path.clear();
    for (Vec2 p : newPath)
    {
        path.push_back(p);
    }
}
