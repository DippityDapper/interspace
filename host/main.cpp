#include "Server.h"

int main(int argc, char* argv[])
{
    Server* server = new Server();

    server->Init();

    while (server->IsRunning())
    {
        server->Run();
    }

    server->Quit();

    delete server;
    return 0;
}