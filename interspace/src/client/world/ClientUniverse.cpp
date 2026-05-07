#include "interspace/client/world/ClientUniverse.hpp"

namespace Interspace::Client
{
    void ClientUniverse::InitUniverse(Client* _client)
    {
        client = _client;
    }

    void ClientUniverse::Update(double delta)
    {
    }
}