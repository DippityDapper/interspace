#include "game/server/PlayerServer.hpp"

namespace Game
{
    PlayerServer::PlayerServer(uint32_t _clientId, Engine::Vec2<uint64_t> _position)
    {
        clientId = _clientId;
        position = _position;
    }
}
