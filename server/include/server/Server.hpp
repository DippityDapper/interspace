#pragma once

#include <thread>
#include <cctype>

#include "server/WorldData.hpp"

namespace Server
{
    class Server
    {
    private:
        static bool running;
        std::unique_ptr<WorldData> worldData = nullptr;

        std::string worldsPath = "../client/worlds";

    public:
        static std::string worldsPathAbs;

    public:
        bool Init(int port, const std::string& _worldName, size_t peerCount, bool localOnly);
        void Update();
        void Clean();
        static void Stop();
    };
}
