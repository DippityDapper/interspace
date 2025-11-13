#include "game/server/ColonistServer.hpp"

namespace Game
{
    ColonistServer::ColonistServer(uint32_t _id, const std::string& _name, const Engine::Vec2<float>& _position)
    {
        id = _id;
        name = _name;
        position = _position;
    }
}
