#include "AStar.h"
#include <queue>
#include <cmath>
#include <algorithm>

std::vector<Vec2> AStar::AStar::FindPath(Vec2 start, Vec2 goal, const Grid grid)
{
    if (!grid.tiles.contains(start))
        return std::vector<Vec2>{};
    if (!grid.tiles.contains(goal))
        return std::vector<Vec2>{};

    std::priority_queue<Node, std::vector<Node>, CompareNode> openQueue{};
    std::map<Vec2, Node> openDict{};
    std::map<Vec2, Node> closedDict{};

    float startHCost = GetHCost(start, goal);
    Node startNode
    {
            start,
            start,
            0,
            startHCost,
            startHCost
    };

    openQueue.push(startNode);
    openDict.emplace(start, startNode);

    while (!openQueue.empty())
    {
        Node current = openQueue.top();
        openQueue.pop();
        openDict.erase(current.position);
        closedDict.emplace(current.position, current);

        if (current.position == goal)
            return ReconstructPath(current, closedDict, grid);

        std::vector<Vec2> directions{};

        if (heuristic == EUCLIDEAN)
        {
            directions =
            {
                    {-1, -1},
                    {0, -1},
                    {1, -1},
                    {-1, 0},
                    {1, 0},
                    {-1, 1},
                    {0, 1},
                    {1, 1},
            };
        }
        if (heuristic == MANHATTAN)
        {
            directions =
                    {
                            {0, -1},
                            {-1, 0},
                            {1, 0},
                            {0, 1},
                    };
        }
        if (heuristic == DIAGONAL)
        {
            directions =
                    {
                            {-1, -1},
                            {1, -1},
                            {-1, 1},
                            {1, 1},
                    };
        }



        for (Vec2 direction : directions)
        {
            float x = direction.x;
            float y = direction.y;

            Vec2 neighborPosition{current.position.x + x, current.position.y + y};

            if (!grid.tiles.contains(neighborPosition))
                continue;
            if (closedDict.contains(neighborPosition))
                continue;

            float stepCost = (x != 0 && y != 0) ? std::sqrt(2) : 1;
            float newGCost = current.gCost + stepCost;
            float newHCost = GetHCost(neighborPosition, goal);
            float newFCost = newGCost + newHCost;

            if (!openDict.contains(neighborPosition) || newGCost < openDict[neighborPosition].gCost)
            {
                Node neighborNode
                {
                    neighborPosition,
                    current.position,
                    newGCost,
                    newHCost,
                    newFCost
                };

                openQueue.push(neighborNode);
                openDict[neighborPosition] = neighborNode;
            }
        }
    }

    return std::vector<Vec2>{};
}

std::vector<Vec2> AStar::AStar::ReconstructPath(Node endNode, std::map<Vec2, Node> closedDict, const Grid grid)
{
    std::vector<Vec2> path{};
    Node current = endNode;
    Vec2 offset{grid.tileSize.x/2, grid.tileSize.y/2};

    while (true)
    {
        path.push_back(grid.LocalToGlobal(current.position) + offset);
        if (current.position == current.parentPosition)
            break;
        current = closedDict[current.parentPosition];
    }

    std::reverse(path.begin(), path.end());
    return path;
}

float AStar::AStar::GetHCost(Vec2 current, Vec2 goal)
{
    switch (heuristic)
    {
        case EUCLIDEAN:
        {
            return std::sqrt(std::pow(current.x - goal.x, 2) + std::pow(current.y - goal.y, 2));
        }
        case MANHATTAN:
        {
            int dx = std::abs(current.x - goal.x);
            int dy = std::abs(current.y - goal.y);
            return dx + dy;
        }
        case DIAGONAL:
        {
            int dx = std::abs(current.x - goal.x);
            int dy = std::abs(current.y - goal.y);
            return std::max(dx, dy);
        }
    }
    return 0;
}

void AStar::AStar::SetHeuristic(Heuristic h)
{
    heuristic = h;
}
