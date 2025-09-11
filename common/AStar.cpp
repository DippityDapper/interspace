#include "AStar.h"
#include <queue>
#include <cmath>
#include <algorithm>
#include <iostream>

std::vector<Position> AStar::AStar::FindPath(Position start, Position goal)
{
    if (!grid.contains(start))
        return std::vector<Position>{};
    if (!grid.contains(goal))
        return std::vector<Position>{};

    std::priority_queue<Node, std::vector<Node>, CompareNode> openQueue{};
    std::map<Position, Node> openDict{};
    std::map<Position, Node> closedDict{};

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
            return ReconstructPath(current, closedDict);

        std::vector<Position> directions{};

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



        for (Position direction : directions)
        {
            float x = direction.x;
            float y = direction.y;

            Position neighborPosition{current.position.x + x, current.position.y + y};

            if (!grid.contains(neighborPosition))
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

    return std::vector<Position>{};
}

std::vector<Position> AStar::AStar::ReconstructPath(Node endNode, std::map<Position, Node> closedDict)
{
    std::vector<Position> path{};
    Node current = endNode;

    while (true)
    {
        path.push_back(current.position);
        if (current.position == current.parentPosition)
            break;
        current = closedDict[current.parentPosition];
    }

    std::reverse(path.begin(), path.end());
    return path;
}

float AStar::AStar::GetHCost(Position current, Position goal)
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

void AStar::AStar::SetGrid(int sizeX, int sizeY)
{
    for (int x = 0; x < sizeX; x++)
    for (int y = 0; y < sizeY; y++)
    {
        Position gridPosition{(float)x, (float)y};
        Tile tile{gridPosition};
        grid.emplace(gridPosition, tile);
    }
}

void AStar::AStar::SetHeuristic(Heuristic h)
{
    heuristic = h;
}
