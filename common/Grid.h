#ifndef SDL3_FIRST_PROJECT_GRID_H
#define SDL3_FIRST_PROJECT_GRID_H

#include <map>
#include "Tile.h"


class Grid
{
private:

private:


public:
    std::map<Vec2, Tile> tiles{};
    Vec2 gridSize{0, 0};
    Vec2 tileSize{0, 0};

public:
    void InitializeGrid(int gridX, int gridY, int tileX, int tileY);
    Vec2 GlobalToLocal(Vec2 position) const;
    Vec2 LocalToGlobal(Vec2 position) const;
};


#endif
