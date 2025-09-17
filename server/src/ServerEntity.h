#ifndef SDL3_FIRST_PROJECT_SERVERENTITY_H
#define SDL3_FIRST_PROJECT_SERVERENTITY_H

#include <vector>
#include "SDL3/SDL.h"
#include "../../common/src/Vec2.h"

class ServerEntity
{
private:
    Vec2 position{0,0};

    float speed = 300;
    std::vector<Vec2> path{};
    Vec2 target{0,0};
    bool isPathing = false;
public:
    ServerEntity(float x, float y);
    void Update(float delta);
    void SetPosition(float x, float y);
    Vec2 GetPosition();
    void SetPath(const std::vector<Vec2>& newPath);
};

#endif
