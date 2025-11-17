#pragma once

#include <map>
#include <vector>

#include "game/server/ChunkServer.hpp"

namespace Game
{
    enum Heuristic
    {
        EUCLIDEAN,
        MANHATTAN,
        DIAGONAL
    };

    struct Node
    {
        Engine::Vec2<uint32_t> position{0,0};
        Engine::Vec2<uint32_t> parentPosition{0,0};

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
        static inline Heuristic heuristic = MANHATTAN;

    private:
        static std::vector<Engine::Vec2<uint32_t>> ReconstructPath(
            const Node& endNode,
            std::map<Engine::Vec2<uint32_t>, Node>& closedDict,
            uint8_t tileSize);

        static float GetHCost(Engine::Vec2<uint32_t> current, Engine::Vec2<uint32_t> goal);

    public:
        static std::vector<Engine::Vec2<uint32_t>> FindPath(
            Engine::Vec2<uint32_t> start,
            Engine::Vec2<uint32_t> goal,
            std::map<Engine::Vec2<uint16_t>, ChunkServer*> areas,
            uint8_t tileSize,
            uint8_t areaSize);

        static void SetHeuristic(Heuristic h);
    };
}
