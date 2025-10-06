#include "client/Area.hpp"

#include "client/Grid.hpp"

namespace Game
{
    Area::Area(int x, int y)
    {
        position.x = x;
        position.y = y;
        grid.Init();
    }

    void Area::RenderTiles()
    {
        for (auto& kvp : grid.tiles)
        {
            Engine::Vec2<int> offset = position * Grid::GRID_SIZE;
            kvp.second->Render(offset.x, offset.y);
        }
    }
}
