#pragma once

#include <map>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>

#include "server/world/Tile.h"
#include "server/world/Grid.h"

namespace Engine
{
    enum Heuristic
    {
        EUCLIDEAN,
        MANHATTAN,
        DIAGONAL
    };

    struct Node
    {
        Vec2<int> position{0,0};
        Vec2<int> parentPosition{0,0};

        float gCost{};
        float hCost{};
        float fCost{};
    };

    struct CompareNode
    {
        bool operator()(const Node& a, const Node& b) const
        {
            if (a.fCost == b.fCost)
                return a.hCost > b.hCost;
            return a.fCost > b.fCost;
        }
    };

    class AStar
    {
    private:
        Heuristic heuristic = MANHATTAN;

    private:
        std::vector<Vec2<int>> ReconstructPath(Node endNode, std::map<Vec2<int>, Node> closedDict, const Grid grid);
        float GetHCost(Vec2<int> current, Vec2<int> goal);

    public:
        std::vector<Vec2<int>> FindPath(Vec2<int> start, Vec2<int> goal, const Grid& grid);
        void SetHeuristic(Heuristic h);
    };
}
