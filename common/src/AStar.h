#ifndef SDL3_FIRST_PROJECT_ASTAR_H
#define SDL3_FIRST_PROJECT_ASTAR_H

#include <map>
#include <vector>
#include "../../client/src/Tile.h"
#include "Grid.h"

namespace AStar
{
    enum Heuristic
    {
        EUCLIDEAN,
        MANHATTAN,
        DIAGONAL
    };

    struct Node
    {
        Vec2 position{0,0};
        Vec2 parentPosition{0,0};

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
        std::vector<Vec2> ReconstructPath(Node endNode, std::map<Vec2, Node> closedDict, const Grid grid);
        float GetHCost(Vec2 current, Vec2 goal);

    public:
        std::vector<Vec2> FindPath(Vec2 start, Vec2 goal, const Grid grid);
        void SetHeuristic(Heuristic h);
    };
}


#endif
