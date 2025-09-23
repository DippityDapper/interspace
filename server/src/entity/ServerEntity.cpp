#include "server/entity/ServerEntity.h"

namespace Game
{
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
            target = (Engine::Vec2<float>)path.front();

            path.erase(path.begin());
        }
        if (isPathing)
        {
            Engine::Vec2 direction = (target - position).Normalized();
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

    Engine::Vec2<float> ServerEntity::GetPosition()
    {
        return position;
    }

    void ServerEntity::SetPath(const std::vector<Engine::Vec2<int>>& newPath)
    {
        path.clear();
        for (Engine::Vec2 p : newPath)
        {
            path.push_back(p);
        }
    }
}
