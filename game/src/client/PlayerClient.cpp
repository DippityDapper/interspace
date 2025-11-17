#include "game/client/PlayerClient.hpp"

namespace Game
{
    PlayerClient::PlayerClient(uint32_t _clientId, Engine::Vec2<uint64_t> _position)
    {
        clientId = _clientId;
        position = _position;
    }
}
