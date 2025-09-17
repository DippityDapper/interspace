#include "Server.h"

int main(int argc, char* argv[])
{
    Server server{};

    server.Init();

    while (server.IsRunning())
    {
        server.HandleEvents();
        server.Update();
    }

    server.Clean();

    return 0;
}