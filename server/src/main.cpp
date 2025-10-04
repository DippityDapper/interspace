#include "server/server/Server.h"

int main(int argc, char* argv[])
{
    Game::Server server{};

    server.Init();
    server.Update();
    server.Clean();

    return 0;
}