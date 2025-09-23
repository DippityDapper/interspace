#include "server/world/Area.h"

namespace Game
{
    void Area::Init()
    {
        grid.InitializeGrid(25, 25, 64, 64);
    }

    void Area::Update(float delta)
    {

    }
}
