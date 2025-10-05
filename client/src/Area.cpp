#include "client/Area.hpp"

#include "client/Grid.hpp"

namespace Game
{
    Area::Area(int x, int y)
    {
        position.x = x;
        position.y = y;
        grid.Init(25, 25);
    }

    void Area::RenderTiles()
    {
        for (auto& kvp : grid.tiles)
        {
            Engine::Vec2<int> gridPosition = position * grid.gridSize;
            kvp.second->Render(gridPosition.x, gridPosition.y);
        }
    }
}
