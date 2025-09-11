#ifndef SDL3_FIRST_PROJECT_ASTAR_H
#define SDL3_FIRST_PROJECT_ASTAR_H

#include <map>
#include <vector>
#include "Tile.h"

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
        Position position;
        Position parentPosition;

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
        std::map<Position, Tile> grid;
        Heuristic heuristic = MANHATTAN;

    private:
        std::vector<Position> ReconstructPath(Node endNode, std::map<Position, Node> closedDict);
        float GetHCost(Position current, Position goal);

    public:
        std::vector<Position> FindPath(Position start, Position goal);
        void SetGrid(int sizeX, int sizeY);
        void SetHeuristic(Heuristic h);
    };
}


#endif
