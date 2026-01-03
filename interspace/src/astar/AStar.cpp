// #include "game/astar/AStar.hpp"
//
// #include <algorithm>
// #include <queue>
//
// namespace Game
// {
//     std::vector<Engine::Vec2<uint32_t>> AStar::FindPath(
//         Engine::Vec2<uint32_t> start,
//         Engine::Vec2<uint32_t> goal,
//         std::map<Engine::Vec2<uint16_t>, ChunkServer*> areas,
//         uint8_t tileSize,
//         uint8_t areaSize)
//     {
//         uint16_t startAreaPositionX = start.x / (tileSize * areaSize);
//         uint16_t startAreaPositionY = start.y / (tileSize * areaSize);
//         Engine::Vec2<uint16_t> startAreaPosition{startAreaPositionX, startAreaPositionY};
//
//         uint16_t goalAreaPositionX = goal.x / (tileSize * areaSize);
//         uint16_t goalAreaPositionY = goal.y / (tileSize * areaSize);
//         Engine::Vec2<uint16_t> goalAreaPosition{goalAreaPositionX, goalAreaPositionY};
//
//         if (!areas.contains(startAreaPosition))
//             return std::vector<Engine::Vec2<uint32_t>>{};
//         if (!areas.contains(goalAreaPosition))
//             return std::vector<Engine::Vec2<uint32_t>>{};
//
//         std::priority_queue<Node, std::vector<Node>, CompareNode> openQueue{};
//         std::map<Engine::Vec2<uint32_t>, Node> openDict{};
//         std::map<Engine::Vec2<uint32_t>, Node> closedDict{};
//
//         float startHCost = GetHCost(start, goal);
//         Node startNode
//         {
//             start,
//             start,
//             0,
//             startHCost,
//             startHCost
//         };
//
//         openQueue.push(startNode);
//         openDict.emplace(start, startNode);
//
//         while (!openQueue.empty())
//         {
//             Node current = openQueue.top();
//             openQueue.pop();
//             openDict.erase(current.position);
//             closedDict.emplace(current.position, current);
//
//             if (current.position == goal)
//                 return ReconstructPath(current, closedDict, tileSize);
//
//             std::vector<Engine::Vec2<int>> directions{};
//
//             if (heuristic == EUCLIDEAN)
//             {
//                 directions =
//                 {
//                     {-1, -1},
//                     {0,  -1},
//                     {1,  -1},
//                     {-1, 0},
//                     {1,  0},
//                     {-1, 1},
//                     {0,  1},
//                     {1,  1},
//                 };
//             }
//             else if (heuristic == MANHATTAN)
//             {
//                 directions =
//                 {
//                     {0,  -1},
//                     {-1, 0},
//                     {1,  0},
//                     {0,  1},
//                 };
//             }
//             else if (heuristic == DIAGONAL)
//             {
//                 directions =
//                 {
//                     {-1, -1},
//                     {1,  -1},
//                     {-1, 1},
//                     {1,  1},
//                 };
//             }
//
//             for (Engine::Vec2 direction: directions)
//             {
//                 int x = direction.x;
//                 int y = direction.y;
//
//                 uint32_t neighborGlobalPositionX = current.position.x + x;
//                 uint32_t neighborGlobalPositionY = current.position.y + y;
//
//                 uint8_t neighborLocalPositionX = neighborGlobalPositionX % areaSize;
//                 uint8_t neighborLocalPositionY = neighborGlobalPositionY % areaSize;
//
//                 uint16_t areaPositionX = neighborGlobalPositionX / (tileSize * areaSize);
//                 uint16_t areaPositionY = neighborGlobalPositionY / (tileSize * areaSize);
//
//                 Engine::Vec2<uint16_t> areaPosition{areaPositionX, areaPositionY};
//                 Engine::Vec2<uint32_t> neighborGlobalPosition{neighborGlobalPositionX, neighborGlobalPositionY};
//                 Engine::Vec2<uint8_t> neighborLocalPosition{neighborLocalPositionX, neighborLocalPositionY};
//
//                 if (!areas.contains(areaPosition) || !areas[areaPosition]->tiles.contains(neighborLocalPosition))
//                     continue;
//                 if (closedDict.contains(neighborGlobalPosition))
//                     continue;
//
//                 float stepCost = (x != 0 && y != 0) ? std::sqrt(2) : 1;
//                 float newGCost = current.gCost + stepCost;
//                 float newHCost = GetHCost(neighborGlobalPosition, goal);
//                 float newFCost = newGCost + newHCost;
//
//                 if (!openDict.contains(neighborGlobalPosition) || newGCost < openDict[neighborGlobalPosition].gCost)
//                 {
//                     Node neighborNode
//                             {
//                                     neighborGlobalPosition,
//                                     current.position,
//                                     newGCost,
//                                     newHCost,
//                                     newFCost
//                             };
//
//                     openQueue.push(neighborNode);
//                     openDict[neighborGlobalPosition] = neighborNode;
//                 }
//             }
//         }
//
//         return std::vector<Engine::Vec2<uint32_t>>{};
//     }
//
//     std::vector<Engine::Vec2<uint32_t>> AStar::ReconstructPath(
//         const Node& endNode,
//         std::map<Engine::Vec2<uint32_t>, Node>& closedDict,
//         uint8_t tileSize)
//     {
//         std::vector<Engine::Vec2<uint32_t>> path{};
//         Node current = endNode;
//         uint32_t tSize = tileSize;
//         Engine::Vec2<uint32_t> offset{tSize / 2, tSize / 2};
//
//         while (true)
//         {
//             path.push_back(current.position + offset);
//             if (current.position == current.parentPosition)
//                 break;
//             current = closedDict[current.parentPosition];
//         }
//
//         path.erase(path.end());
//         std::ranges::reverse(path);
//         return path;
//     }
//
//     float AStar::GetHCost(Engine::Vec2<uint32_t> current, Engine::Vec2<uint32_t> goal)
//     {
//         uint32_t dx = (current.x > goal.x) ? (current.x - goal.x) : (goal.x - current.x);
//         uint32_t dy = (current.y > goal.y) ? (current.y - goal.y) : (goal.y - current.y);
//
//         switch (heuristic)
//         {
//         case EUCLIDEAN:
//             {
//                 double ddx = (double)dx;
//                 double ddy = (double)dy;
//                 return std::sqrt(ddx * ddx + ddy * ddy);
//             }
//
//         case MANHATTAN:
//             return (float)(dx + dy);
//
//         case DIAGONAL:
//             return (float)std::max(dx, dy);
//         }
//
//         return 0.0f;
//     }
//
//     void AStar::SetHeuristic(Heuristic h)
//     {
//         heuristic = h;
//     }
// }
